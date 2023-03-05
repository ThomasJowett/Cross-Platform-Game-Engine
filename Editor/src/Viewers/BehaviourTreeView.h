#pragma once

#include "ViewerManager.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/ISaveable.h"
#include "Interfaces/IUndoable.h"
#include "ImGui/Node Editor/imgui_node_editor.h"

namespace BehaviourTree
{
class BehaviourTree;
class Node;
}

class BehaviourTreeView
	:public View, public ICopyable, public ISaveable, public IUndoable
{
	enum class PinKind
	{
		Output,
		Input
	};

	struct Node;
	struct Pin
	{
		NodeEditor::PinId id;
		Node* node;
		PinKind kind;

		Pin(int id, Node* node, PinKind kind)
			:id(id), node(node), kind(kind)
		{}
	};

	struct Node
	{
		NodeEditor::NodeId id;
		std::string name;
		Ref<Pin> input;
		Ref<Pin> output;

		std::vector<std::string> decorators;

		Ref<BehaviourTree::Node> btNodeRef;

		Node(int id, std::string_view name, Ref<BehaviourTree::Node> nodeRef)
			:id(id), name(name)
		{}
	};

	struct Link
	{
		NodeEditor::LinkId id;
		NodeEditor::PinId startPinId;
		NodeEditor::PinId endPinId;

		Link(NodeEditor::LinkId id, NodeEditor::PinId startPinId, NodeEditor::PinId endPinId)
			:id(id), startPinId(startPinId), endPinId(endPinId)
		{}
	};

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

	void DrawNode(Ref<BehaviourTree::Node> node);

	int GetNextId()
	{
		return m_NextId++;
	}

private:

	void BuildNodeList();
	BehaviourTreeView::Node* BuildNode(Ref<BehaviourTree::Node> btNode);
	bool* m_Show;

	std::filesystem::path m_Filepath;

	bool m_Dirty = false;

	NodeEditor::EditorContext* m_NodeEditorContext = nullptr;

	Ref<BehaviourTree::BehaviourTree> m_BehaviourTree;
	Ref<BehaviourTree::BehaviourTree> m_LocalBehaviourTree;

	int m_NextId = 1;

	std::vector<Node> m_Nodes;
	std::vector<Link> m_Links;

	Pin* m_NewLinkPin = nullptr;
	Pin* m_NewNodeLinkPin = nullptr;
};
