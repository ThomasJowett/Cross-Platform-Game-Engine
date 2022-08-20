#include "Lua_NodeEditor.h"
#include "IconsFontAwesome5.h"

#include <imgui/imgui_internal.h>

void LuaNodeBuilder::Begin(NodeEditor::NodeId id)
{
	m_HasHeader = false;
	m_HeaderMin = m_HeaderMax = ImVec2();
	NodeEditor::PushStyleVar(NodeEditor::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

	NodeEditor::BeginNode(id);

	ImGui::PushID(id.AsPointer());
	m_CurrentNodeId = id;
	SetStage(Stage::Begin);
}

void LuaNodeBuilder::End()
{
	SetStage(Stage::End);

	NodeEditor::EndNode();

	ImGui::PopID();

	NodeEditor::PopStyleVar();

	SetStage(Stage::Invalid);
}

void LuaNodeBuilder::Header(const ImVec4& colour)
{
	m_HeaderColor = ImColor(colour);
	SetStage(Stage::Header);
}

void LuaNodeBuilder::EndHeader()
{
	SetStage(Stage::Content);
}

void LuaNodeBuilder::Input(NodeEditor::PinId id)
{
	if (m_CurrentStage == Stage::Begin)
		SetStage(Stage::Content);

	const bool applyPadding = (m_CurrentStage == Stage::Input);

	SetStage(Stage::Input);

	//if (applyPadding)
	//  ImGui::Spring(0);

	Pin(id, NodeEditor::PinKind::Input);
}

void LuaNodeBuilder::EndInput()
{
	EndPin();
}

void LuaNodeBuilder::Middle()
{
	if (m_CurrentStage == Stage::Begin)
		SetStage(Stage::Content);

	SetStage(Stage::Middle);
}

void LuaNodeBuilder::Output(NodeEditor::PinId id)
{
}

void LuaNodeBuilder::EndOutput()
{
}

bool LuaNodeBuilder::SetStage(Stage stage)
{
	if (stage == m_CurrentStage)
		return false;

	Stage oldStage = m_CurrentStage;
	m_CurrentStage = stage;

	ImVec2 cursor;
	switch (oldStage)
	{
	case LuaNodeBuilder::Stage::Invalid:
		break;
	case LuaNodeBuilder::Stage::Begin:
		break;
	case LuaNodeBuilder::Stage::Header:
		m_HeaderMin = ImGui::GetItemRectMin();
		m_HeaderMax = ImGui::GetItemRectMax();
		break;
	case LuaNodeBuilder::Stage::Content:
		break;
	case LuaNodeBuilder::Stage::Input:
		break;
	case LuaNodeBuilder::Stage::Output:
		break;
	case LuaNodeBuilder::Stage::Middle:
		break;
	case LuaNodeBuilder::Stage::End:
		break;
	default:
		break;
	}
	return false;
}

void LuaNodeBuilder::Pin(NodeEditor::PinId id, NodeEditor::PinKind kind)
{
}

void LuaNodeBuilder::EndPin()
{
}

LuaNodeEditor::LuaNodeEditor()
{
}

void LuaNodeEditor::SetFilepath(std::string_view filepath)
{
	NodeEditor::Config config;
	config.SettingsFile = filepath;
	m_NodeEditorContext = NodeEditor::CreateEditor(&config);
}

void LuaNodeEditor::Destroy()
{
	NodeEditor::DestroyEditor(m_NodeEditorContext);
}

void LuaNodeEditor::Render()
{
	NodeEditor::SetCurrentEditor(m_NodeEditorContext);

	NodeEditor::Begin("Node Editor");

	/* temp code start */
	int uniqueId = 1;
	NodeEditor::BeginNode(uniqueId++);
	ImGui::Text("Node A");
	NodeEditor::BeginPin(uniqueId++, NodeEditor::PinKind::Input);
	ImGui::Text("-> In");
	NodeEditor::EndPin();
	ImGui::SameLine();
	NodeEditor::BeginPin(uniqueId++, NodeEditor::PinKind::Output);
	ImGui::Text("Out ->");
	NodeEditor::EndPin();
	NodeEditor::EndNode();
	/* temp code end*/

	LuaNodeBuilder builder;

	for (auto& node : m_Nodes)
	{
		builder.Begin(node.id);
		for (auto& input : node.inputs)
		{
			float alpha = ImGui::GetStyle().Alpha;
			if (m_NewLinkPin && !CanCreateLink(m_NewLinkPin, &input) && &input != m_NewLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			builder.Input(input.id);
			DrawPin(input, IsPinLinked(input.id), (int)(alpha * 255));
			builder.EndInput();
		}

		for (auto& output : node.outputs)
		{
			builder.Output(output.id);
			builder.EndOutput();
		}
		builder.End();
	}

	ImVec2 openPopupPosition = ImGui::GetMousePos();

	NodeEditor::Suspend();

	if (NodeEditor::ShowNodeContextMenu(&m_ContextNodeId))
		ImGui::OpenPopup("Node Context Menu");
	else if (NodeEditor::ShowPinContextMenu(&m_ContextPinId))
		ImGui::OpenPopup("Pin Context Menu");
	else if (NodeEditor::ShowLinkContextMenu(&m_ContextLinkId))
		ImGui::OpenPopup("Link Context Menu");
	else if (NodeEditor::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		m_NewNodeLinkPin = nullptr;
	}
	NodeEditor::Resume();

	NodeEditor::Suspend();
	if (ImGui::BeginPopup("Node Context Menu"))
	{
		Node* node = FindNode(m_ContextNodeId);
		if (node)
		{
			if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete"))
				NodeEditor::DeleteNode(m_ContextNodeId);
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Pin Context Menu"))
	{
		Pin* pin = FindPin(m_ContextPinId);

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Link Context Menu"))
	{
		Link* link = FindLink(m_ContextLinkId);

		if (link)
		{
			if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete"))
				NodeEditor::DeleteLink(m_ContextLinkId);
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Create New Node"))
	{
		ImGui::Text("Create Node");

		Node* node = nullptr;
		if (ImGui::MenuItem("Input Action"))
			node = SpawnNode();

		if (node)
		{
			BuildNodes();

			m_CreateNewNode = false;

			NodeEditor::SetNodePosition(node->id, openPopupPosition);

			if (Pin* startPin = m_NewNodeLinkPin)
			{
				for (Pin& pin : startPin->kind == PinKind::Input ? node->outputs : node->inputs)
				{
					if (CanCreateLink(startPin, &pin))
					{
						Pin* endPin = &pin;
						if (startPin->kind == PinKind::Input)
							std::swap(startPin, endPin);

						m_Links.emplace_back(Link(GetNextId(), startPin->id, endPin->id));
						m_Links.back().colour = GetPinColour(startPin->type);

						break;
					}
				}
			}
		}
		ImGui::EndPopup();
	}
	NodeEditor::Resume();

	NodeEditor::End();

	NodeEditor::SetCurrentEditor(nullptr);
}

bool LuaNodeEditor::IsPinLinked(NodeEditor::PinId id)
{
	if (!id)
		return false;

	for (auto& link : m_Links)
		if (link.startPinId == id || link.endPinId == id)
			return true;

	return false;
}

bool LuaNodeEditor::CanCreateLink(Pin* a, Pin* b)
{
	if (!a || !b || a == b || a->kind == b->kind || a->type != b->type || a->node == b->node)
		return false;
	return true;
}

LuaNodeEditor::Node* LuaNodeEditor::FindNode(NodeEditor::NodeId id)
{
	for (Node& node : m_Nodes)
		if (node.id == id)
			return &node;
	return nullptr;
}

LuaNodeEditor::Pin* LuaNodeEditor::FindPin(NodeEditor::PinId id)
{
	for (Node& node : m_Nodes)
	{
		for (Pin& pin : node.inputs)
		{
			if (pin.id == id)
				return &pin;
		}

		for (Pin& pin : node.outputs)
		{
			if (pin.id == id)
				return &pin;
		}
	}
	return nullptr;
}

LuaNodeEditor::Link* LuaNodeEditor::FindLink(NodeEditor::LinkId id)
{
	for (Link& link : m_Links)
		if (link.id == id)
			return &link;
	return nullptr;
}

int LuaNodeEditor::GetNextId()
{
	return m_NextId++;
}

void LuaNodeEditor::BuildNode(Node* node)
{
	for (auto& input : node->inputs)
	{
		input.node = node;
		input.kind = PinKind::Input;
	}

	for (auto& output : node->outputs)
	{
		output.node = node;
		output.kind = PinKind::Output;
	}
}

void LuaNodeEditor::BuildNodes()
{
	for (auto& node : m_Nodes)
		BuildNode(&node);
}

LuaNodeEditor::Node* LuaNodeEditor::SpawnNode()
{
	m_Nodes.emplace_back(GetNextId(), "Input Action", ImColor(255, 128, 128));
	m_Nodes.back().outputs.emplace_back(GetNextId(), "", PinType::Execution);

	BuildNode(&m_Nodes.back());
	return &m_Nodes.back();
}

ImColor LuaNodeEditor::GetPinColour(PinType type)
{
	switch (type)
	{
	case LuaNodeEditor::PinType::Execution:		return ImColor(255, 255, 255);
	case LuaNodeEditor::PinType::Bool:			return ImColor(220, 48, 48);
	case LuaNodeEditor::PinType::Int:			return ImColor(68, 201, 156);
	case LuaNodeEditor::PinType::Float:			return ImColor(147, 226, 74);
	case LuaNodeEditor::PinType::String:		return ImColor(124, 21, 153);
	case LuaNodeEditor::PinType::Vec2:			return ImColor(255, 202, 35);
	case LuaNodeEditor::PinType::Vec3:			return ImColor(35, 202, 255);
	case LuaNodeEditor::PinType::Quaternion:	return ImColor(160, 180, 255);
	case LuaNodeEditor::PinType::Colour:		return ImColor(0, 89, 203);
	case LuaNodeEditor::PinType::Object:		return ImColor(0, 203, 89);
	case LuaNodeEditor::PinType::Enum:			return ImColor(0, 111, 111);
	default:
		break;
	}
}

void LuaNodeEditor::DrawPin(const Pin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor colour = GetPinColour(pin.type);
	colour.Value.w = alpha / 255.0f;
	switch (pin.type)
	{
	case PinType::Execution:	iconType = IconType::Arrow; break;
	case PinType::Bool:			iconType = IconType::Circle; break;
	case PinType::Int:			iconType = IconType::Circle; break;
	case PinType::Float:		iconType = IconType::Circle; break;
	case PinType::String:		iconType = IconType::Circle; break;
	case PinType::Object:		iconType = IconType::Circle; break;
	case PinType::Vec2:			iconType = IconType::Circle; break;
	case PinType::Vec3:			iconType = IconType::Circle; break;
	case PinType::Quaternion:	iconType = IconType::Circle; break;
	case PinType::Colour:		iconType = IconType::Circle; break;
	case PinType::Enum:			iconType = IconType::Circle; break;
	default:
		break;
	}

	if (ImGui::IsRectVisible(ImVec2(m_PinSize, m_PinSize)))
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (iconType == IconType::Arrow)
		{
		}
	}

	ImGui::Dummy(ImVec2(m_PinSize, m_PinSize));
}

void LuaNodeEditor::Undo(int asteps)
{
}

void LuaNodeEditor::Redo(int asteps)
{
}

bool LuaNodeEditor::CanUndo() const
{
	return false;
}

bool LuaNodeEditor::CanRedo() const
{
	return false;
}

void LuaNodeEditor::Copy()
{
}

void LuaNodeEditor::Cut()
{
}

void LuaNodeEditor::Paste()
{
}

void LuaNodeEditor::Duplicate()
{
}

void LuaNodeEditor::Delete()
{
}

bool LuaNodeEditor::HasSelection() const
{
	return NodeEditor::GetSelectedObjectCount() > 0;
}

void LuaNodeEditor::SelectAll()
{
	for (Node& node : m_Nodes)
		NodeEditor::SelectNode(node.id, true);

	for (Link& link : m_Links)
		NodeEditor::SelectLink(link.id, true);
}

bool LuaNodeEditor::IsReadOnly() const
{
	return false;
}