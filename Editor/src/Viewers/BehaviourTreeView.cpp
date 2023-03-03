#include "BehaviourTreeView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "AI/BehaviorTree.h"
#include "AI/BehaviourTreeSerializer.h"

BehaviourTreeView::BehaviourTreeView(bool* show, const std::filesystem::path& filepath)
	:View("BehaviourTreeView"), m_Show(show), m_Filepath(filepath)
{
}

void BehaviourTreeView::OnAttach()
{
	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());

	NodeEditor::Config config;
	m_NodeEditorContext = NodeEditor::CreateEditor(&config);

	m_BehaviourTree = BehaviourTree::Serializer::Deserialize(m_Filepath);
	if (m_BehaviourTree)
		m_LocalBehaviourTree = CreateRef<BehaviourTree::BehaviourTree>(*m_BehaviourTree);
	else
		ViewerManager::CloseViewer(m_Filepath);
}

void BehaviourTreeView::OnDetach()
{
	NodeEditor::DestroyEditor(m_NodeEditorContext);
	m_NodeEditorContext = nullptr;
}

void BehaviourTreeView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_Dirty)
		{
			ImGui::OpenPopup(("Save Prompt " + m_WindowName).c_str());
		}

		if (ImGui::BeginPopupModal(("Save Prompt " + m_WindowName).c_str()))
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
				m_Dirty = false;
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

		ViewerManager::CloseViewer(m_Filepath);
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

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
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT" Undo", "Ctrl-Z", nullptr, CanUndo()))
					Undo(1);
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT" Redo", "Ctrl-Y", nullptr, CanRedo()))
					Redo(1);
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl-X", nullptr, HasSelection()))
					Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, HasSelection()))
					Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
					Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, HasSelection()))
					Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, HasSelection()))
					Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_ARROW_POINTER" Select all", "Ctrl-A", nullptr))
					SelectAll();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		NodeEditor::SetCurrentEditor(m_NodeEditorContext);
		NodeEditor::Begin("Node Editor");

		NodeEditor::End();
		NodeEditor::SetCurrentEditor(nullptr);
	}

	ImGui::End();
}

void BehaviourTreeView::Copy()
{
}

void BehaviourTreeView::Cut()
{
}

void BehaviourTreeView::Paste()
{
}

void BehaviourTreeView::Duplicate()
{
}

void BehaviourTreeView::Delete()
{
}

bool BehaviourTreeView::HasSelection() const
{
	return false;
}

void BehaviourTreeView::SelectAll()
{
}

bool BehaviourTreeView::IsReadOnly() const
{
	return false;
}

void BehaviourTreeView::Save()
{
	BehaviourTree::Serializer::Serialize(m_Filepath, m_LocalBehaviourTree.get());
	m_BehaviourTree = BehaviourTree::Serializer::Deserialize(m_Filepath);
	m_LocalBehaviourTree = CreateRef<BehaviourTree::BehaviourTree>(*m_BehaviourTree);
	m_Dirty = false;
}

void BehaviourTreeView::SaveAs()
{
}

bool BehaviourTreeView::NeedsSaving()
{
	return false;
}

void BehaviourTreeView::Undo(int asteps)
{
}

void BehaviourTreeView::Redo(int asteps)
{
}

bool BehaviourTreeView::CanUndo() const
{
	return false;
}

bool BehaviourTreeView::CanRedo() const
{
	return false;
}
