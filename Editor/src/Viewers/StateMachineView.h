#pragma once

#include "ViewerManager.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/ISaveable.h"
#include "Interfaces/IUndoable.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/Node Editor/imgui_node_editor.h"
#include "imgui/imgui_internal.h"

#include <optional>

class StateMachine;

class StateMachineView : public View, public ICopyable, public ISaveable, public IUndoable
{
private:
	struct Node
	{
		int id = 0;
		int inputPinId = 0; // Drop target while a transition is being dragged
		int outputPinId = 0; // Handle that transitions are dragged from
		bool isEntry = false;
		ImVec2 position;
		std::string name;
		std::filesystem::path scriptPath;
	};

	// A transition between states, or the arrow from the entry to the initial state
	struct Link
	{
		int id = 0;
		int fromId = 0;
		int toId = 0;
		int fromPinId = 0;
		int toPinId = 0;
	};

	struct EditorState
	{
		std::vector<Node> nodes;
		std::vector<Link> links;
	};

	class UndoRecord
	{
	public:
		UndoRecord(const EditorState& before, const EditorState& after) : m_Before(before), m_After(after) {}

		void Undo(StateMachineView* editor);
		void Redo(StateMachineView* editor);

		EditorState m_Before;
		EditorState m_After;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

public:
	StateMachineView(bool* show, const std::filesystem::path& filepath);
	~StateMachineView() = default;

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
	int GetNextId() { return m_NextId++; }

	void DrawNodeEditor();
	void DrawNode(const Node& node);
	void DrawLinkPins(const Node& node);
	std::string GetNodeSummary(const Node& node) const;
	void DrawProperties();
	void DrawStateProperties(Node& node);
	void DrawTransitionProperties(const Link& link);
	void FinishEdit(const EditorState& before, bool changed, bool finished);
	void HandleCreate();
	void HandleDelete();
	void SyncNodePositions();

	Node* SpawnState(const ImVec2& position, const std::string& name);
	Node* FindNode(int id);
	const Node* FindNode(int id) const;
	Node* FindNodeByPin(NodeEditor::PinId pinId, bool* isOutput = nullptr);
	Node* GetEntry();
	Link* FindLink(int id);
	const Link* FindLink(int fromId, int toId) const;
	const Node* GetInitialState() const;
	ImRect GetNodeRect(const Node& node) const;
	std::string MakeUniqueName(const std::string& base) const;
	bool CanLink(const Node* from, const Node* to) const;
	void AddLink(int fromId, int toId);
	void SetInitialState(int nodeId);
	void RenameState(int nodeId, const std::string& newName);
	void RemoveNodes(const std::vector<int>& nodeIds, const std::vector<int>& linkIds);

	std::vector<int> GetSelectedNodeIds() const;
	std::vector<int> GetSelectedLinkIds() const;

	std::string SerializeNodes(const std::vector<int>& nodeIds);
	bool PasteNodes(const std::string& text, std::optional<ImVec2> position);

	void Commit(const EditorState& before);
	void RestoreState(const EditorState& state);
	bool IsDirty() const { return m_UndoIndex != m_SavedUndoIndex; }

	void BuildGraph(Ref<StateMachine> stateMachine);
	void AutoLayout();
	Ref<StateMachine> BuildStateMachine() const;

	bool* m_Show;

	std::filesystem::path m_Filepath; // Key the viewer is registered under
	std::filesystem::path m_SavePath;

	NodeEditor::EditorContext* m_NodeEditorContext = nullptr;

	int m_NextId = 1;

	EditorState m_State;
	std::vector<int> m_PendingPositions;
	std::vector<int> m_PendingSelection;
	bool m_ClearSelection = false;
	bool m_NavigateToContent = true;
	bool m_IsCreatingLink = false;

	UndoBuffer m_UndoBuffer;
	int m_UndoIndex = 0;
	int m_SavedUndoIndex = 0;
	std::optional<EditorState> m_EditBefore;

	int m_RenamingNodeId = 0;
	std::string m_RenameBuffer;

	std::optional<std::string> m_PendingPaste;
	bool m_PasteAtMouse = false;
	ImVec2 m_CanvasMin;
	ImVec2 m_CanvasMax;

	int m_ContextNodeId = 0;
	int m_ContextLinkId = 0;
	ImVec2 m_NewNodePosition;
};
