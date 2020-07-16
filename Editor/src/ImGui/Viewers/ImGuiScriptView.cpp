#include "ImGuiScriptView.h"
#include "Fonts/Fonts.h"
#include "ImGui/ImGuiDockSpace.h"
#include "IconsFontAwesome5.h"
#include "FileSystem/FileDialog.h"

ImGuiScriptView::ImGuiScriptView(bool* show, std::filesystem::path filepath)
	:Layer("ScriptView"), m_Show(show), m_FilePath(filepath)
{
}

void ImGuiScriptView::OnAttach()
{
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
}

void ImGuiScriptView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_TextEditor.NeedsSaving())
		{
			CLIENT_DEBUG("needs saving");
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
			ImGuiDockSpace::SetFocussedWindow(this);
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

			//if (ImGui::BeginMenu("View"))
			//{
			//	if (ImGui::MenuItem("Dark palette"))
			//		m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
			//	if (ImGui::MenuItem("Light palette"))
			//		m_TextEditor.SetPalette(TextEditor::GetLightPalette());
			//	if (ImGui::MenuItem("Retro blue Palette"))
			//		m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
			//	ImGui::EndMenu();
			//}
		}
		ImGui::EndMenuBar();

		ImGui::PushFont(Fonts::Consolas);
		m_TextEditor.Render("TextEditor");
		ImGui::PopFont();
	}

	ImGui::End();
}

void ImGuiScriptView::Save()
{
	if (!IsReadOnly())
		m_TextEditor.SaveTextToFile(m_FilePath);
}

void ImGuiScriptView::SaveAs()
{
	auto ext = m_FilePath.extension();
	m_FilePath = SaveAsDialog(L"Save As...", m_FilePath.extension().c_str());
	if (!m_FilePath.has_extension())
		m_FilePath.replace_extension(ext);
	Save();
}

TextEditor::LanguageDefinition ImGuiScriptView::DetermineLanguageDefinition()
{
	auto ext = m_FilePath.extension();

	if (ext == ".lua")
		return TextEditor::LanguageDefinition::Lua();
	else if (ext == ".cpp" || ext == ".hpp")
		return TextEditor::LanguageDefinition::CPlusPlus();
	else if (ext == ".h" || ext == ".c")
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
