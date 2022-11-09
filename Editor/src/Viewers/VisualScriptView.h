#pragma once

#include <map>
#include <filesystem>

#include "Core/Layer.h"

#include "ImGui/Node Editor/Lua_NodeEditor.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

#include "ViewerManager.h"

class VisualSriptView
	:public View, public ICopyable, public IUndoable, public ISaveable
{
	struct Variable
	{
		enum class Type
		{
			Bool,
			Int,
			Float,
			String,
			Vec2,
			Vec3,
			Quaternion,
			Colour,
			Object,
			Enum
		} type;

		std::string defaultValue = "false";
	};
public:
	VisualSriptView(bool* show, const std::filesystem::path& filepath);
	~VisualSriptView() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnImGuiRender() override;

	// Inherited via IUndoable
	virtual void Undo(int asteps) override { m_LuaNodeEditor.Undo(asteps); }
	virtual void Redo(int asteps) override { m_LuaNodeEditor.Redo(asteps); }
	virtual bool CanUndo() const override { return m_LuaNodeEditor.CanUndo(); }
	virtual bool CanRedo() const override { return m_LuaNodeEditor.CanRedo(); }

	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override { return m_Dirty; }

	// Inherited via ICopyable
	virtual void Copy() override { m_LuaNodeEditor.Copy(); }
	virtual void Cut() override { m_LuaNodeEditor.Cut(); }
	virtual void Paste() override { m_LuaNodeEditor.Paste(); }
	virtual void Duplicate() override { m_LuaNodeEditor.Duplicate(); }
	virtual void Delete() override { m_LuaNodeEditor.Delete(); }
	virtual bool HasSelection() const override { return m_LuaNodeEditor.HasSelection(); }
	virtual void SelectAll() override { m_LuaNodeEditor.SelectAll(); }
	virtual bool IsReadOnly() const override { return m_LuaNodeEditor.IsReadOnly(); }

private:
	bool* m_Show;

	std::filesystem::path m_Filepath;

	bool m_Dirty = false;

	LuaNodeEditor m_LuaNodeEditor;

	std::map<std::string, Variable> m_Variables;
};
