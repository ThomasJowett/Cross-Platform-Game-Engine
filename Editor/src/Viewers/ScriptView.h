#pragma once
#include "Engine.h"

#include "ImGui/ImGuiTextEditor.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

class ScriptView
	:public Layer, public ICopyable, public IUndoable, public ISaveable
{
public:
	ScriptView(bool* show, const std::filesystem::path& filepath);
	~ScriptView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;

	virtual void Copy() override { m_TextEditor.Copy(); }
	virtual void Cut() override { m_TextEditor.Cut(); }
	virtual void Paste() override { m_TextEditor.Paste(); }
	virtual void Duplicate() override { m_TextEditor.Duplicate(); }
	virtual void Delete() override { m_TextEditor.Delete(); }

	virtual bool IsReadOnly() const override { return m_TextEditor.IsReadOnly(); }

	virtual bool CanUndo() const override { return m_TextEditor.CanUndo(); };
	virtual bool CanRedo() const override { return m_TextEditor.CanRedo(); };
	virtual void Undo(int asteps = 1) { m_TextEditor.Undo(asteps); }
	virtual void Redo(int asteps = 1) { m_TextEditor.Redo(asteps); }

	virtual bool HasSelection() const override { return m_TextEditor.HasSelection(); }
	virtual void SelectAll() override { m_TextEditor.SelectAll(); }

	virtual void Save() override;
	virtual void SaveAs() override;
private:
	TextEditor::LanguageDefinition DetermineLanguageDefinition();
	void ParseLuaScript();
private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	std::string m_WindowName;

	TextEditor m_TextEditor;
};
