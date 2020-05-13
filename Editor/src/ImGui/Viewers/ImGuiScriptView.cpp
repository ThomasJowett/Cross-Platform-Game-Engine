#include "ImGuiScriptView.h"
#include "Fonts/Fonts.h"

ImGuiScriptView::ImGuiScriptView(bool* show, std::filesystem::path filepath)
	:m_Show(show), m_FilePath(filepath)
{
}

void ImGuiScriptView::OnAttach()
{
	m_WindowName = m_FilePath.filename().string();

	TextEditor::LanguageDefinition lang = DetermineLanguageDefinition();

	m_TextEditor.SetLanguageDefinition(lang);

	std::ifstream file(m_FilePath);

	if (file.good())
	{
		std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		m_TextEditor.SetText(str);
	}
}

void ImGuiScriptView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	ImGui::PushFont(Fonts::Consolas);

	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(m_WindowName.c_str(), m_Show, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar))
	{
		bool readOnly = m_TextEditor.IsReadOnly();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "Ctrl-S", nullptr, !readOnly))
				{
					m_TextEditor.SaveTextToFile(m_FilePath);
				}
			ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Read-Only mode", nullptr, &readOnly))
					m_TextEditor.SetReadOnly(readOnly);
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, !readOnly && m_TextEditor.CanUndo()))
					m_TextEditor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !readOnly && m_TextEditor.CanRedo()))
					m_TextEditor.Redo();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Cut();
				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, m_TextEditor.HasSelection()))
					m_TextEditor.Copy();
				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr && !readOnly))
					m_TextEditor.Paste();
				if (ImGui::MenuItem("Duplicate", "Ctrl-D", nullptr, !readOnly))
					m_TextEditor.Duplicate();
				if (ImGui::MenuItem("Delete", "Del", nullptr, m_TextEditor.HasSelection() && !readOnly))
					m_TextEditor.Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
					m_TextEditor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(m_TextEditor.GetTotalLines(), 0));
			ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark palette"))
					m_TextEditor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light palette"))
					m_TextEditor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro blue Palette"))
					m_TextEditor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		m_TextEditor.Render("TextEditor");
	}
	ImGui::End();
	ImGui::PopFont();
}

TextEditor::LanguageDefinition ImGuiScriptView::DetermineLanguageDefinition()
{
	auto ext = m_FilePath.extension();

	if (ext == ".lua")
		return TextEditor::LanguageDefinition::Lua();
	else if (ext == ".cpp" || ext == ".h" || ext == ".c" || ext == ".hpp")
		return TextEditor::LanguageDefinition::CPlusPlus();
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
