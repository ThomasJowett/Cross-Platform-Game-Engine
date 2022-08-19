#pragma once
#include <vector>
#include <string>

#include "ImGui/Node Editor/imgui_node_editor.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ICopyable.h"

class LuaNodeBuilder
{
public:
	LuaNodeBuilder() = default;
	void Begin(NodeEditor::NodeId id);
	void End();

	void Header(const ImVec4& colour = ImVec4(1, 1, 1, 1));
	void EndHeader();

	void Input(NodeEditor::PinId id);
	void EndInput();

	void Middle();

	void Output(NodeEditor::PinId id);
	void EndOutput();

private:
	enum class Stage
	{
		Invalid,
		Begin,
		Header,
		Content,
		Input,
		Output,
		Middle,
		End
	};

	bool SetStage(Stage stage);

	void Pin(NodeEditor::PinId id, NodeEditor::PinKind kind);
	void EndPin();

	NodeEditor::NodeId m_CurrentNodeId = 0;
	Stage m_CurrentStage = Stage::Invalid;
	ImU32 m_HeaderColor;
	ImVec2 m_NodeMin;
	ImVec2 m_NodeMax;
	ImVec2 m_HeaderMin;
	ImVec2 m_HeaderMax;
	ImVec2 m_ContentMin;
	ImVec2 m_ContentMax;
	bool m_HasHeader = false;
};

class LuaNodeEditor : public IUndoable, public ICopyable
{
public:
	enum class PinType
	{
		Execution,
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
	};

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
		std::string name;
		PinType type;
		PinKind kind;

		Pin(int id, std::string_view name, PinType type)
			:id(id), node(nullptr), name(name), type(type), kind(PinKind::Input)
		{}
	};

	struct Node
	{
		NodeEditor::NodeId id;
		std::string name;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;
		ImColor colour;
		ImVec2 size;

		std::string state;
		std::string savedState;

		Node(int id, std::string_view name, ImColor colour = ImColor(255, 255, 255))
			:id(id), name(name), colour(colour), size(0, 0)
		{}
	};

	struct Link
	{
		NodeEditor::LinkId id;
		NodeEditor::PinId startPinId;
		NodeEditor::PinId endPinId;

		ImColor colour;

		Link(NodeEditor::LinkId id, NodeEditor::PinId startPinId, NodeEditor::PinId endPinId)
			:id(id), startPinId(startPinId), endPinId(endPinId), colour(255, 255, 255)
		{}
	};

	LuaNodeEditor();

	void SetFilepath(std::string_view filepath);
	void Destroy();

	void Render();

	// Inherited via IUndoable
	virtual void Undo(int asteps = 1) override;
	virtual void Redo(int asteps = 1) override;
	virtual bool CanUndo() const override;
	virtual bool CanRedo() const override;

	// Inherited via ICopyable
	virtual void Copy() override;
	virtual void Cut() override;
	virtual void Paste() override;
	virtual void Duplicate() override;
	virtual void Delete() override;
	virtual bool HasSelection() const override;
	virtual void SelectAll() override;
	virtual bool IsReadOnly() const override;

	bool NeedsSaving() { return m_Dirty; }

private:
	bool IsPinLinked(NodeEditor::PinId id);
	bool CanCreateLink(Pin* a, Pin* b);
private:

	NodeEditor::EditorContext* m_NodeEditorContext = nullptr;
	std::vector<Node> m_Nodes;
	std::vector<Link> m_Links;

	Pin* m_NewLinkPin = nullptr;
	Pin* m_NewNodeLinkPin = nullptr;

	bool m_Dirty = false;
};