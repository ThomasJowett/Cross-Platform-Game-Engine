#pragma once

#include "ViewerManager.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/ISaveable.h"
#include "Interfaces/IUndoable.h"

class BehaviourTreeView
	:public View, public ICopyable, public ISaveable, public IUndoable
{
public:
	BehaviourTreeView(bool* show, const std::filesystem::path& filepath);
	~BehaviourTreeView() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnImGuiRender() override;

	// Inherited via ICopyable
	virtual void Copy() override;
	virtual void Cut() override;
	virtual void Paste() override;
	virtual void Duplicate() override;
	virtual void Delete() override;
	virtual bool HasSelection() const override;
	virtual void SelectAll() override;
	virtual bool IsReadOnly() const override;

	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override;

	// Inherited via IUndoable
	virtual void Undo(int asteps) override;
	virtual void Redo(int asteps) override;
	virtual bool CanUndo() const override;
	virtual bool CanRedo() const override;

private:
	bool* m_Show;

	std::filesystem::path m_Filepath;

	bool m_Dirty = false;


};
