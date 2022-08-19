#include "VisualScriptView.h"
#include "MainDockSpace.h"
#include "IconsFontAwesome5.h"

VisualSriptView::VisualSriptView(bool* show, const std::filesystem::path& filepath)
	:Layer("VisualScriptView"), m_Show(show), m_Filepath(filepath)
{
}

void VisualSriptView::OnAttach()
{
	m_LuaNodeEditor.SetFilepath(m_Filepath.string());

	m_WindowName = ICON_FA_PROJECT_DIAGRAM + std::string(" " + m_Filepath.filename().string());
}

void VisualSriptView::OnDetach()
{
}

void VisualSriptView::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(900, 900), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_SAVE" Save", "Ctrl + S"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_UNDO" Undo", "Ctrl-Z", nullptr, m_LuaNodeEditor.CanUndo()))
					m_LuaNodeEditor.Undo();
				if (ImGui::MenuItem(ICON_FA_REDO" Redo", "Ctrl-Y", nullptr, m_LuaNodeEditor.CanRedo()))
					m_LuaNodeEditor.Redo();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_CUT" Cut", "Ctrl-X", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
					m_LuaNodeEditor.Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete", "Del", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_MOUSE_POINTER" Select all", "Ctrl-A", nullptr))
					m_LuaNodeEditor.SelectAll();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		m_LuaNodeEditor.Render();
	}

	ImGui::End();
}

void VisualSriptView::Save()
{
}

void VisualSriptView::SaveAs()
{
}