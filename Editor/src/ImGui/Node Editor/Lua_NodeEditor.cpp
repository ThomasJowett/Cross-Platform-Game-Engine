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

void LuaNodeEditor::Render()
{
  NodeEditor::Begin("Node Editor");

  for (auto& node : m_Nodes)
  {
    for (auto& input : node.inputs)
    {
      float alpha = ImGui::GetStyle().Alpha;
    }
  }
}