#include "ScriptView.h"
#include "Fonts/Fonts.h"
#include "MainDockSpace.h"
#include "IconsFontAwesome5.h"

#include "Scripting/Lua/LuaManager.h"
#include "FileSystem/FileDialog.h"
#include "Core/Settings.h"
#include "ViewerManager.h"

ScriptView::ScriptView(bool* show, const std::filesystem::path& filepath)
	:Layer("ScriptView"), m_Show(show), m_FilePath(filepath)
{
}

void ScriptView::OnAttach()
{
	if (!std::filesystem::exists(m_FilePath))
	{
		ViewerManager::CloseViewer(m_FilePath);
		return;
	}

	m_WindowName = ICON_FA_FILE_CODE + std::string(" " + m_FilePath.filename().string());

	TextEditor::LanguageDefinition lang = DetermineLanguageDefinition();

	m_TextEditor.SetLanguageDefinition(lang);

	std::ifstream file(m_FilePath);

	if (file.good())
	{
		std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		m_TextEditor.SetText(str);
		m_TextEditor.SetFilePath(m_FilePath);
	}
	Settings::SetDefaultBool("TextEditor", "ShowWhiteSpace", true);
	m_TextEditor.SetShowWhitespaces(Settings::GetBool("TextEditor", "ShowWhiteSpace"));

	if (m_FilePath.extension() == ".lua")
	{
		ParseLuaScript();
	}
}

void ScriptView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_TextEditor.NeedsSaving())
		{
			ImGui::OpenPopup("Save Prompt");
		}

		if (ImGui::BeginPopupModal("Save Prompt"))
		{
			ImGui::Text("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				m_TextEditor.SaveTextToFile(m_FilePath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				m_TextEditor.DiscardAllChanges();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*m_Show = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;

	if (m_TextEditor.NeedsSaving())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		bool readOnly = m_TextEditor.IsReadOnly();


		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_SAVE" Save", "Ctrl-S", nullptr, !readOnly))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl-Shift-S"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_UNDO" Undo", "Ctrl-Z", nullptr, !readOnly && m_TextEditor.CanUndo()))
					m_TextEditor.Undo();
				if (ImGui::MenuItem(ICON_FA_REDO" Redo", "Ctrl-Y", nullptr, !readOnly && m_TextEditor.CanRedo()))
					m_TextEditor.Redo();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_CUT" Cut", "Ctrl-X", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, m_TextEditor.HasSelection()))
					m_TextEditor.Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr && !readOnly))
					m_TextEditor.Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, !readOnly))
					m_TextEditor.Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete", "Del", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_MOUSE_POINTER" Select all", "Ctrl-A", nullptr))
					m_TextEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_TextEditor.GetTotalLines(), 0));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				bool showWhiteSpace = m_TextEditor.IsShowingWhitespaces();
				if (ImGui::MenuItem("Show white Space", "", &showWhiteSpace))
				{
					m_TextEditor.SetShowWhitespaces(showWhiteSpace);
					Settings::SetBool("TextEditor", "ShowWhiteSpace", showWhiteSpace);
				}

				bool colourize = m_TextEditor.IsColorizerEnabled();
				if (ImGui::MenuItem("Colourize Text", "", &colourize))
				{
					m_TextEditor.SetColorizerEnable(colourize);
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGui::PushFont(Fonts::Consolas);
		m_TextEditor.Render("TextEditor");
		ImGui::PopFont();
	}

	ImGui::End();
}

void ScriptView::Save()
{
	if (!IsReadOnly())
		m_TextEditor.SaveTextToFile(m_FilePath);

	if (m_FilePath.extension() == ".lua")
	{
		ParseLuaScript();
	}
}

void ScriptView::SaveAs()
{
	auto ext = m_FilePath.extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_FilePath.extension().string()));
	if(dialogPath)
	{
		m_FilePath =  dialogPath.value();
		if (!m_FilePath.has_extension())
			m_FilePath.replace_extension(ext);
		Save();
	}
}

TextEditor::LanguageDefinition ScriptView::DetermineLanguageDefinition()
{
	auto ext = m_FilePath.extension();

	if (ext == ".lua")
		return TextEditor::LanguageDefinition::Lua();
	else if (ext == ".cpp" || ext == ".hpp")
		return TextEditor::LanguageDefinition::CPlusPlus();
	else if (ext == ".h" || ext == ".c" || ext == ".cs")
		return TextEditor::LanguageDefinition::C();
	else if (ext == ".hlsl" || ext == ".fx")
		return TextEditor::LanguageDefinition::HLSL();
	else if (ext == ".glsl" || ext == ".frag" || ext == ".vert" || ext == ".tesc" || ext == ".tese" || ext == ".geom" || ext == ".comp")
		return TextEditor::LanguageDefinition::GLSL();
	else if (ext == ".sql")
		return TextEditor::LanguageDefinition::SQL();
	else if (ext == ".as")
		return TextEditor::LanguageDefinition::AngelScript();

	return TextEditor::LanguageDefinition();
}

void ScriptView::ParseLuaScript()
{
	Ref<sol::environment> m_SolEnvironment = CreateRef<sol::environment>(LuaManager::GetState(), sol::create, LuaManager::GetState().globals());
	sol::protected_function_result result = LuaManager::GetState().script_file(m_FilePath.string(), *m_SolEnvironment, sol::script_pass_on_error);

	TextEditor::ErrorMarkers errorMarkers;
	if (!result.valid())
	{
		sol::error error = result;
		std::string errorStr = error.what();
		int line = 1;
		auto linepos = errorStr.find(".lua:");
		std::string errorLine = errorStr.substr(linepos + 5); //+4 .lua: + 1
		auto lineposEnd = errorLine.find(":");
		errorLine = errorLine.substr(0, lineposEnd);
		line = std::stoi(errorLine);
		errorStr = errorStr.substr(linepos + errorLine.size() + lineposEnd + 4); //+4 .lua:

		errorMarkers.insert({ line, errorStr });
		m_TextEditor.SetErrorMarkers(errorMarkers);
	}
	else
	{
		m_TextEditor.SetErrorMarkers(errorMarkers);
	}
}
