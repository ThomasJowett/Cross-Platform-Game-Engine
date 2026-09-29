#pragma once

#include "ViewerManager.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/ISaveable.h"
#include "Interfaces/IUndoable.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/Node Editor/imgui_node_editor.h"

#include "math/Vector3f.h"

#include <optional>

namespace tinyxml2
{
	class XMLElement;
}

namespace BehaviourTree
{
	class BehaviourTree;
	class Blackboard;
	class Node;
} // namespace BehaviourTree

class BehaviourTreeView : public View, public ICopyable, public ISaveable, public IUndoable
{
public:
	enum class NodeType
	{
		Root,
		Selector,
		Sequence,
		StatefulSelector,
		MemSequence,
		ParallelSequence,
		BlackboardBool,
		BlackboardCompare,
		Succeeder,
		Failer,
		Inverter,
		Repeater,
		UntilSuccess,
		UntilFailure,
		Wait,
		CustomTask,
		RandomWait,
		SetBlackboard,
		EmitSignal
	};

	enum class NodeCategory
	{
		Root,
		Composite,
		Decorator,
		Task
	};

	enum class BlackboardType
	{
		Bool,
		Int,
		Float,
		Double,
		String,
		Vec2,
		Vec3
	};

private:
	struct BlackboardEntry
	{
		std::string key;
		BlackboardType type = BlackboardType::Bool;

		// Only the value matching the type is used
		bool boolValue = false;
		int intValue = 0;
		double numberValue = 0.0; // Float and Double
		std::string stringValue;
		Vector3f vectorValue; // Vec2 uses x and y
	};

	struct Node
	{
		int id = 0;
		int inputPinId = 0;
		int outputPinId = 0;
		NodeType type = NodeType::Root;
		ImVec2 position;

		// Parameters, only the ones relevant to the type are used
		float waitTime = 1.0f;
		std::filesystem::path scriptPath;
		std::string key1 = "key";
		std::string key2 = "key2";
		bool flag = true; // BlackboardBool IsSet, BlackboardCompare IsEqual
		int limit = 0;
		bool usePolicy = true;
		bool successOnAll = true;
		bool failOnAll = true;
		int minSuccess = 1;
		int minFail = 1;
		float minTime = 0.5f;
		float maxTime = 1.5f;
		std::string signalName = "Signal";
		BlackboardEntry setValue; // SetBlackboard key and value
	};

	struct Link
	{
		int id = 0;
		int parentId = 0;
		int childId = 0;
	};

	struct EditorState
	{
		std::vector<Node> nodes;
		std::vector<Link> links;
		std::vector<BlackboardEntry> blackboard;
	};

	class UndoRecord
	{
	public:
		UndoRecord(const EditorState& before, const EditorState& after) : m_Before(before), m_After(after) {}

		void Undo(BehaviourTreeView* editor);
		void Redo(BehaviourTreeView* editor);

		EditorState m_Before;
		EditorState m_After;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

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
	int GetNextId() { return m_NextId++; }

	void DrawNodeEditor();
	void DrawNode(const Node& node);
	std::string GetNodeSummary(const Node& node);
	std::string GetChildOrderLabel(int nodeId);
	void DrawCreateNodePopup();
	void DrawProperties();
	void DrawBlackboard();
	bool BlackboardKeyCombo(const char* label, std::string& key, BlackboardType type, bool anyType = false);
	void DrawBlackboardValue(BlackboardEntry& entry, bool& changed, bool& finished);
	void FinishEdit(const EditorState& before, bool changed, bool finished);
	void HandleCreate();
	void HandleDelete();
	void SyncNodePositions();

	Node* SpawnNode(NodeType type, const ImVec2& position);
	Node* FindNode(int id);
	Node* FindNodeByPin(NodeEditor::PinId pinId, bool* isOutput = nullptr);
	Link* FindLinkToChild(int childId);
	std::vector<int> GetChildren(int parentId) const;
	int GetParent(int childId) const;
	bool IsAncestor(int ancestorId, int nodeId) const;
	bool CanLink(const Node* parent, const Node* child) const;
	void AddLink(int parentId, int childId);
	void RemoveNodes(const std::vector<int>& nodeIds, const std::vector<int>& linkIds);

	BlackboardEntry* FindBlackboardEntry(const std::string& key);
	std::string MakeUniqueBlackboardKey(const std::string& base);
	void RenameBlackboardEntry(size_t index, const std::string& newKey);

	std::vector<int> GetSelectedNodeIds() const;
	std::vector<int> GetSelectedLinkIds() const;

	std::string SerializeNodes(const std::vector<int>& nodeIds);
	bool PasteNodes(const std::string& text, std::optional<ImVec2> position);

	void Commit(const EditorState& before);
	void RestoreState(const EditorState& state);
	bool IsDirty() const { return m_UndoIndex != m_SavedUndoIndex; }

	void BuildGraph(Ref<BehaviourTree::BehaviourTree> behaviourTree);
	int BuildGraphNode(Ref<BehaviourTree::Node> btNode);
	void AutoLayout();
	Ref<BehaviourTree::BehaviourTree> BuildBehaviourTree();
	Ref<BehaviourTree::Node> BuildBehaviourTreeNode(int nodeId, BehaviourTree::BehaviourTree* behaviourTree);

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

	UndoBuffer m_UndoBuffer;
	int m_UndoIndex = 0;
	int m_SavedUndoIndex = 0;
	std::optional<EditorState> m_EditBefore;

	int m_RenamingEntry = -1;
	std::string m_RenameBuffer;

	std::optional<std::string> m_PendingPaste;
	bool m_PasteAtMouse = false;
	ImVec2 m_CanvasMin;
	ImVec2 m_CanvasMax;

	int m_ContextNodeId = 0;
	int m_ContextLinkId = 0;

	int m_NewNodeLinkPinNodeId = 0;
	bool m_NewNodeLinkFromOutput = false;
	ImVec2 m_NewNodePosition;
	bool m_CreateNewNode = false;
};
