#include "ScriptView.h"
#include "Fonts/Fonts.h"
#include "ImGui/ImGuiTextEditor.h"
#include "MainDockSpace.h"
#include "IconsFontAwesome6.h"

#include "FileSystem/FileDialog.h"
#include "Core/Settings.h"
#include "ViewerManager.h"
#include "Scripting/Lua/LuaManager.h"
#include "Scene/Entity.h"
#include "Scene/Components/LuaScriptComponent.h"
#include "Scene/AssetManager.h"
#include "imgui.h"
#include <cstddef>
#include <cstring>
#include <string>

ScriptView::ScriptView(bool* show, const std::filesystem::path& filepath)
	:View("ScriptView"), m_Show(show), m_FilePath(filepath)
{
}

void ScriptView::OnAttach()
{
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_FilePath);
	if (!std::filesystem::exists(absolutePath))
	{
		ViewerManager::CloseViewer(m_FilePath);
		return;
	}

	m_Script = AssetManager::GetAsset<LuaScript>(m_FilePath);

	m_WindowName = ICON_FA_FILE_CODE + std::string(" " + m_FilePath.filename().string());

	TextEditor::LanguageDefinition lang = DetermineLanguageDefinition();

	if (lang.mName == "Lua")
	{
		const auto& identifiers = LuaManager::GetIdentifiers();
		for (auto& [keyword, definition] : identifiers) {
			TextEditor::Identifier id;
			id.mDeclaration = definition;
			lang.mIdentifiers.insert(std::make_pair(keyword, id));
		}
	}

	m_TextEditor.SetLanguageDefinition(lang);

	std::ifstream file(absolutePath);

	if (file.good())
	{
		std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		m_TextEditor.SetText(str);
		m_TextEditor.SetFilePath(absolutePath);
	}
	Settings::SetDefaultBool("TextEditor", "ShowWhiteSpace", true);
	m_TextEditor.SetShowWhitespaces(Settings::GetBool("TextEditor", "ShowWhiteSpace"));

	if (m_FilePath.extension() == ".lua")
	{
		ParseLuaScript();
	}
}

void ScriptView::OnDetach()
{
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
			ImGui::TextUnformatted("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				Save();
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
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			MainDockSpace::SetFocussedWindow(this);

			bool ctrl = ImGui::GetIO().ConfigMacOSXBehaviors ? ImGui::GetIO().KeySuper : ImGui::GetIO().KeyCtrl;
			bool shift = ImGui::GetIO().KeyShift;

			if (ctrl && ImGui::IsKeyPressed(ImGuiKey_F))
			{
				m_ShowSearch = true;
				m_FocusSearch = true;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				m_ShowSearch = false;
			}
			else if (m_ShowSearch && ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				if (shift)
					FindPrevious();
				else
				 	FindNext();
			}
		}

		bool readOnly = m_TextEditor.IsReadOnly();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save", "Ctrl + S", nullptr, !readOnly))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT" Undo", "Ctrl-Z", nullptr, !readOnly && m_TextEditor.CanUndo()))
					m_TextEditor.Undo();
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT" Redo", "Ctrl-Y", nullptr, !readOnly && m_TextEditor.CanRedo()))
					m_TextEditor.Redo();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl-X", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, m_TextEditor.HasSelection()))
					m_TextEditor.Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr && !readOnly))
					m_TextEditor.Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, !readOnly))
					m_TextEditor.Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Delete();
				ImGui::Separator();//---------------------------------------------------------------
				if (ImGui::MenuItem(ICON_FA_MAGNIFYING_GLASS " Find", "Ctrl-F"))
				{
					m_ShowSearch = true;
					m_FocusSearch = true;
				}
				ImGui::Separator();//---------------------------------------------------------------
				if (ImGui::MenuItem(ICON_FA_ARROW_POINTER" Select all", "Ctrl-A", nullptr))
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

		if (m_ShowSearch)
		{
			if (m_FocusSearch)
			{
				ImGui::SetKeyboardFocusHere();
				m_FocusSearch = false;
			}

			ImGui::PushItemWidth(300.0f);
			if (ImGui::InputTextWithHint("##SearchBox", "Find...", m_SearchBuffer, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				FindNext();
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_ARROW_UP))
			{
				FindPrevious();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Find Previous (Shift+Enter)");

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_ARROW_DOWN))
			{
				FindNext();
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Find Next (Enter)");

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_XMARK))
			{
				m_ShowSearch = false;
			}

			ImGui::Separator();
		}

		ImGui::PushFont(Fonts::Consolas);
		m_TextEditor.Render("TextEditor");
		ImGui::PopFont();

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			MainDockSpace::SetFocussedWindow(this);
		}
	}

	ImGui::End();
}

void ScriptView::OnEvent(Event& event)
{
	PROFILE_FUNCTION();
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<LuaErrorEvent>([this](LuaErrorEvent& luaErrorEvent)
		{
			if (luaErrorEvent.GetFile() == m_Script->GetFilepath()) {
				TextEditor::ErrorMarkers errorMarkers;
				errorMarkers.emplace(luaErrorEvent.GetLine(), luaErrorEvent.GetErrorMessage());
				m_TextEditor.SetErrorMarkers(errorMarkers);
			}
			return false;
		});
}

void ScriptView::Save()
{
	if (!IsReadOnly()) {
		std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_FilePath);
		m_TextEditor.SaveTextToFile(absolutePath);
	}

	if (m_FilePath.extension() == ".lua")
	{
		ParseLuaScript();
	}

	m_Script->Load(m_FilePath);
}

void ScriptView::SaveAs()
{
	auto ext = m_FilePath.extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_FilePath.extension().string()));
	if (dialogPath)
	{
		m_FilePath = dialogPath.value();
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
	TextEditor::ErrorMarkers errorMarkers;
	m_TextEditor.SetErrorMarkers(errorMarkers);
	Scene testScene("");
	
	Entity entity = testScene.CreateEntity("lua script");
	
	LuaScriptComponent& luaComp = entity.AddComponent<LuaScriptComponent>(m_FilePath);
	luaComp.ParseScript(entity);
}

void ScriptView::FindNext() {
	if (strlen(m_SearchBuffer) == 0)
		return;

	std::string searchStr = m_SearchBuffer;
	auto lines = m_TextEditor.GetTextLines();
	auto cursor = m_TextEditor.GetCursorPosition();

	for (size_t i = cursor.mLine; i < lines.size(); ++i)
	{
		size_t startPos = (i == cursor.mLine) ? cursor.mColumn : 0;
		size_t pos = lines[i].find(searchStr, startPos);

		if (pos != std::string::npos)
		{
			m_TextEditor.SetSelection(TextEditor::Coordinates((int)i, (int)pos), TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			m_TextEditor.SetCursorPosition(TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			return;
		}
	}

	// wrap around
	for (size_t i = 0; i <= cursor.mLine && i < lines.size(); ++i)
	{
		size_t pos = lines[i].find(searchStr, 0);

		if (pos != std::string::npos)
		{
			m_TextEditor.SetSelection(TextEditor::Coordinates((int)i, (int)pos), TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			m_TextEditor.SetCursorPosition(TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			return;
		}
	}
}

void ScriptView::FindPrevious() {
	if (strlen(m_SearchBuffer) == 0)
		return;

	std::string searchStr = m_SearchBuffer;
	auto lines = m_TextEditor.GetTextLines();
	auto cursor = m_TextEditor.GetCursorPosition();

	if (m_TextEditor.HasSelection())
	{
		cursor.mColumn -= (int)searchStr.length();
		if (cursor.mColumn < 0)
			cursor.mColumn = 0;
	}

	if (cursor.mLine < lines.size())
	{
		std::string linePrefix = lines[cursor.mLine].substr(0, std::max(0, cursor.mColumn));
		size_t pos = linePrefix.rfind(searchStr);
		if (pos != std::string::npos)
		{
			m_TextEditor.SetSelection(TextEditor::Coordinates(cursor.mLine, (int)pos), TextEditor::Coordinates(cursor.mLine, (int)(pos + searchStr.length())));
			m_TextEditor.SetCursorPosition(TextEditor::Coordinates(cursor.mLine, (int)(pos + searchStr.length())));
			return;
		}
	}

	for (int i = cursor.mLine - 1; i >= 0; --i)
	{
		size_t pos = lines[i].rfind(searchStr);
		if (pos != std::string::npos)
		{
			m_TextEditor.SetSelection(TextEditor::Coordinates((int)i, (int)pos), TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			m_TextEditor.SetCursorPosition(TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			return;
		}
	}

	// Wrap around to end
	for (int i = (int)lines.size() - 1; i >= cursor.mLine; --i)
	{
		size_t pos = lines[i].rfind(searchStr);
		if (pos != std::string::npos)
		{
			m_TextEditor.SetSelection(TextEditor::Coordinates((int)i, (int)pos), TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			m_TextEditor.SetCursorPosition(TextEditor::Coordinates((int)i, (int)(pos + searchStr.length())));
			return;
		}
	}
}
