#pragma once

#include <filesystem>
#include "Core/Layer.h"

#include "Interfaces/ISaveable.h"
#include "Interfaces/IUndoable.h"
#include "Physics/PhysicsMaterial.h"

class PhysicsMaterialView :
	public Layer, public ISaveable, public IUndoable
{
public:
	PhysicsMaterialView(bool* show, std::filesystem::path filepath);
	~PhysicsMaterialView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
	
	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override { return m_Dirty; }

	// Inherited via IUndoable
	virtual void Undo(int asteps) override;
	virtual void Redo(int asteps) override;
	virtual bool CanUndo() const override;
	virtual bool CanRedo() const override;

private:
	bool* m_Show;

	std::filesystem::path m_FilePath;
	std::string m_windowName;

	Ref<PhysicsMaterial> m_PhysicsMaterial;

	bool m_Dirty = false;
};
