#include "BehaviourTreeView.h"

#include "IconsFontAwesome6.h"

BehaviourTreeView::BehaviourTreeView(bool* show, const std::filesystem::path& filepath)
	:View("BehaviourTreeView"), m_Show(show), m_Filepath(filepath)
{
}

void BehaviourTreeView::OnAttach()
{
	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());
}

void BehaviourTreeView::OnDetach()
{

}

void BehaviourTreeView::OnImGuiRender()
{
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
