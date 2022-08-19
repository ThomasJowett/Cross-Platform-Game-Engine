#include "Lua_NodeEditor.h"

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
	NodeEditor::End();
	/* temp code end*/

	for (auto& node : m_Nodes)
	{
		for (auto& input : node.inputs)
		{
			float alpha = ImGui::GetStyle().Alpha;
		}
	}

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
	return false;
}

void LuaNodeEditor::SelectAll()
{
}

bool LuaNodeEditor::IsReadOnly() const
{
	return false;
}