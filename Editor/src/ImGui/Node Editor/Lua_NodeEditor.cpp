#include "Lua_NodeEditor.h"
#include "IconsFontAwesome5.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <fstream>

#include "crude_json.h"

namespace json = crude_json;

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

  NodeEditor::BeginPin(id, NodeEditor::PinKind::Input);
}

void LuaNodeBuilder::EndInput()
{
  NodeEditor::EndPin();
}

void LuaNodeBuilder::Middle()
{
  if (m_CurrentStage == Stage::Begin)
    SetStage(Stage::Content);

  SetStage(Stage::Middle);
}

void LuaNodeBuilder::Output(NodeEditor::PinId id)
{
  if (m_CurrentStage == Stage::Begin)
    SetStage(Stage::Content);

  const auto applyPadding = (m_CurrentStage == Stage::Output);

  SetStage(Stage::Output);

  NodeEditor::BeginPin(id, NodeEditor::PinKind::Output);
}

void LuaNodeBuilder::EndOutput()
{
  NodeEditor::EndPin();
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

LuaNodeEditor::LuaNodeEditor()
{
}

struct NodeSetting
{
  NodeEditor::NodeId nodeId;
  std::string data;

  NodeSetting(NodeEditor::NodeId nodeId)
    : nodeId(nodeId) {}
};

void LuaNodeEditor::SetFilepath(std::string_view filepath)
{
  NodeEditor::Config config;
  config.SettingsFile = filepath;

  config.UserPointer = this;

  std::string data;

  std::ifstream file(filepath.data());
  if (file)
  {
    file.seekg(0, std::ios_base::end);
    auto size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios_base::beg);

    data.reserve(size);
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  }

  auto settingsValue = json::value::parse(data);

  if (settingsValue.is_discarded())
    return;

  if (!settingsValue.is_object())
    return;

  //auto deserializeVariable = [](const std::string& str)
  //{
  //  auto separator = str.find_first_of(':');
  //  auto idStart = str.c_str() + ((separator != std::string::npos) ? separator + 1 : 0);
  //  auto id = reinterpret_cast<void*>(strtoull(idStart, nullptr, 10));
  //  if (str.compare(0, separator, "node") == 0)
  //    return ObjectId(NodeId(id));
  //  else if (str.compare(0, separator, "link") == 0)
  //    return ObjectId(LinkId(id));
  //  else if (str.compare(0, separator, "pin") == 0)
  //    return ObjectId(PinId(id));
  //  else
  //    // fallback to old format
  //    return ObjectId(NodeId(id)); //return ObjectId();
  //};

  //auto& variablesValue = settingsValue["variables"];
  //if (variablesValue.is_object())
  //{
  //  for (auto& variable : variablesValue.get<json::object>())
  //  {

  //    //auto nodeSettings = FindNode(id);
  //    //if (!nodeSettings)
  //    //  nodeSettings = result.AddNode(id);
  //  }
  //}

  //m_Data = &data[0];

  //config.LoadSettings = [](char* data, void* userPointer) -> size_t
  //{
  //  static_cast<LuaNodeEditor*>(userPointer)->GetSettingsData();
  //};

  config.LoadNodeSettings = [](NodeEditor::NodeId nodeId, char* data, void* userPointer) -> size_t
  {
    Node* node = static_cast<LuaNodeEditor*>(userPointer)->FindNode(nodeId);
  
  	if (!node)
  		return 0;
  
  	if (data != nullptr)
  		memcpy(data, node->state.data(), node->state.size());
    return node->state.size();
  };

  config.SaveNodeSettings = [](NodeEditor::NodeId nodeId, const char* data, size_t size, NodeEditor::SaveReasonFlags reason, void* userPointer) -> bool
  {
    Node* node = static_cast<LuaNodeEditor*>(userPointer)->FindNode(nodeId);
    if (!node)
      return false;
    
    node->state.assign(data, size);
  
    return true;
  };

  m_NodeEditorContext = NodeEditor::CreateEditor(&config);
}

void LuaNodeEditor::Destroy()
{
  NodeEditor::DestroyEditor(m_NodeEditorContext);
  m_NodeEditorContext = nullptr;
}

void LuaNodeEditor::Render()
{
  NodeEditor::SetCurrentEditor(m_NodeEditorContext);
  NodeEditor::Begin("Node Editor");

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
      if (!input.name.empty())
      {
        ImGui::SameLine();
        ImGui::TextUnformatted(input.name.c_str());
      }
      builder.EndInput();
    }

    for (auto& output : node.outputs)
    {
      float alpha = ImGui::GetStyle().Alpha;
      if (m_NewLinkPin && !CanCreateLink(m_NewLinkPin, &output) && &output != m_NewLinkPin)
        alpha = alpha * (48.0f / 255.0f);

      builder.Output(output.id);
      if (!output.name.empty())
      {
        ImGui::TextUnformatted(output.name.c_str());
        ImGui::SameLine();
      }
      DrawPin(output, IsPinLinked(output.id), (int)(alpha * 255));
      builder.EndOutput();
    }
    builder.End();
  }

  for (auto& link : m_Links)
  {
    NodeEditor::Link(link.id, link.startPinId, link.endPinId, link.colour, 2.0f);
  }

  if (!m_CreateNewNode)
  {
    if (NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
    {
      auto showlabel = [](const char* label, ImColor colour)
      {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
        ImVec2 size = ImGui::CalcTextSize(label);

        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(style.ItemSpacing.x, -style.ItemSpacing.y));

        ImVec2 rectMin = ImGui::GetCursorScreenPos() - style.FramePadding;
        ImVec2 rectMax = ImGui::GetCursorScreenPos() + size + style.FramePadding;

        ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, colour, size.y * 0.15f);
        ImGui::TextUnformatted(label);
      };

      NodeEditor::PinId startPinId = 0, endPinId = 0;
      if (NodeEditor::QueryNewLink(&startPinId, &endPinId))
      {
        Pin* startPin = FindPin(startPinId);
        Pin* endPin = FindPin(endPinId);

        m_NewLinkPin = startPin ? startPin : endPin;

        if (startPin->kind == PinKind::Input)
        {
          std::swap(startPin, endPin);
          std::swap(startPinId, endPinId);
        }

        if (startPin && endPin)
        {
          if (endPin == startPin)
          {
            NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
          }
          else if (endPin->kind == startPin->kind)
          {
            showlabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
            NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
          }
          else if (endPin->type != startPin->type)
          {
            showlabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
            NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
          }
          else
          {
            showlabel("+ Create Link", ImColor(32, 45, 32, 180));
            if (NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
            {
              m_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
              m_Links.back().colour = GetPinColour(startPin->type);
            }
          }
        }
      }

      NodeEditor::PinId pinId = 0;
      if (NodeEditor::QueryNewNode(&pinId))
      {
        m_NewLinkPin = FindPin(pinId);
        if (m_NewLinkPin)
          showlabel("+ Create Node", ImColor(32, 45, 32, 180));

        if (NodeEditor::AcceptNewItem())
        {
          m_CreateNewNode = true;
          m_NewNodeLinkPin = FindPin(pinId);
          m_NewLinkPin = nullptr;
          NodeEditor::Suspend();
          ImGui::OpenPopup("Create New Node");
          NodeEditor::Resume();
        }
      }
    }
    else
      m_NewLinkPin = nullptr;

    NodeEditor::EndCreate();

    if (NodeEditor::BeginDelete())
    {
      NodeEditor::LinkId linkId = 0;
      while (NodeEditor::QueryDeletedLink(&linkId))
      {
        if (NodeEditor::AcceptDeletedItem())
        {
          std::vector<Link>::iterator id = std::find_if(m_Links.begin(), m_Links.end(),
            [linkId](auto& link) { return link.id == linkId; });
          if (id != m_Links.end())
            m_Links.erase(id);
        }
      }

      NodeEditor::NodeId nodeId = 0;
      while (NodeEditor::QueryDeletedNode(&nodeId))
      {
        if (NodeEditor::AcceptDeletedItem())
        {
          std::vector<Node>::iterator id = std::find_if(m_Nodes.begin(), m_Nodes.end(),
            [nodeId](auto& node) { return node.id == nodeId; });
          if (id != m_Nodes.end())
            m_Nodes.erase(id);
        }
      }
    }
    NodeEditor::EndDelete();
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
  else
    m_CreateNewNode = false;
  NodeEditor::Resume();

  NodeEditor::End();

  NodeEditor::SetCurrentEditor(nullptr);
}

char* LuaNodeEditor::GetSettingsData()
{
  return m_Data;
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
  m_Nodes.back().inputs.emplace_back(GetNextId(), "Input", PinType::Execution);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "Output", PinType::Execution);

  m_Nodes.back().inputs.emplace_back(GetNextId(), "test", PinType::Bool);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test2", PinType::Float);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test3", PinType::Int);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test4", PinType::String);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test5", PinType::Vec2);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test6", PinType::Vec3);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test7", PinType::Quaternion);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test8", PinType::Colour);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test9", PinType::Object);
  m_Nodes.back().inputs.emplace_back(GetNextId(), "test10", PinType::Enum);

  m_Nodes.back().outputs.emplace_back(GetNextId(), "output", PinType::Bool);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output2", PinType::Float);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output3", PinType::Int);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output4", PinType::String);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output5", PinType::Vec2);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output6", PinType::Vec3);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output7", PinType::Quaternion);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output8", PinType::Colour);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output9", PinType::Object);
  m_Nodes.back().outputs.emplace_back(GetNextId(), "output10", PinType::Enum);

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
  case PinType::Execution:	iconType = IconType::Arrow;	 break;
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
  default:					return;
  }

  ImVec2 pinSize(m_PinSize, m_PinSize);

  if (ImGui::IsRectVisible(pinSize))
  {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImColor innerColour(ImColor(32, 32, 32, alpha));

    ImRect rect = ImRect(cursorPos, cursorPos + pinSize);
    float rect_w = rect.Max.x - rect.Min.x;
    float outline_scale = rect_w / 24.0f;
    if (iconType == IconType::Arrow)
    {
      const auto origin_scale = rect_w / 24.0f;

      const auto offset_x = 1.0f * origin_scale;
      const auto offset_y = 0.0f * origin_scale;
      const auto margin = (connected ? 2.0f : 2.0f) * origin_scale;
      const auto rounding = 0.1f * origin_scale;
      const auto tip_round = 0.7f; // percentage of triangle edge (for tip)
      //const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
      const auto canvas = ImRect(
        rect.Min.x + margin + offset_x,
        rect.Min.y + margin + offset_y,
        rect.Max.x - margin + offset_x,
        rect.Max.y - margin + offset_y);
      const auto canvas_x = canvas.Min.x;
      const auto canvas_y = canvas.Min.y;
      const auto canvas_w = canvas.Max.x - canvas.Min.x;
      const auto canvas_h = canvas.Max.y - canvas.Min.y;

      const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
      const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
      const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
      const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
      const auto center_y = (top + bottom) * 0.5f;
      //const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

      const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
      const auto tip_right = ImVec2(right, center_y);
      const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

      drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
      drawList->PathBezierCurveTo(
        ImVec2(left, top),
        ImVec2(left, top),
        ImVec2(left, top) + ImVec2(rounding, 0));
      drawList->PathLineTo(tip_top);
      drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
      drawList->PathBezierCurveTo(
        tip_right,
        tip_right,
        tip_bottom + (tip_right - tip_bottom) * tip_round);
      drawList->PathLineTo(tip_bottom);
      drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
      drawList->PathBezierCurveTo(
        ImVec2(left, bottom),
        ImVec2(left, bottom),
        ImVec2(left, bottom) - ImVec2(0, rounding));

      if (!connected)
      {
        if (innerColour & 0xFF000000)
          drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColour);

        drawList->PathStroke(colour, true, 2.0f * outline_scale);
      }
      else
        drawList->PathFillConvex(colour);
    }
    else
    {
      auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
      auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
      auto rect_center = ImVec2(rect_center_x, rect_center_y);
      auto triangleStart = rect_center_x + 0.32f * rect_w;

      auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

      rect.Min.x += rect_offset;
      rect.Max.x += rect_offset;
      rect_center.x += rect_offset * 0.5f;

      const auto c = rect_center;

      const auto extra_segments = static_cast<int>(2 * outline_scale);

      if (!connected)
      {
        const auto r = 0.5f * rect_w / 2.0f - 0.5f;
        if (innerColour & 0xFF000000)
          drawList->AddCircleFilled(c, r, innerColour, 12 + extra_segments);
        drawList->AddCircle(c, r, colour, 12 + extra_segments, 2.0f * outline_scale);
      }
      else
        drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, colour, 12 + extra_segments);

      const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

      float rect_h = rect.Max.y - rect.Min.y;

      drawList->AddTriangleFilled(
        ImVec2(ceilf(triangleTip), rect.Min.y + rect_h * 0.5f),
        ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
        ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
        colour);
    }
  }

  ImGui::Dummy(ImVec2(m_PinSize, m_PinSize));
}

void LuaNodeEditor::Undo(int steps)
{
  while (CanUndo() && steps-- > 0)
    m_UndoBuffer[--m_UndoIndex].Undo(this);

  if (!CanUndo())
    m_Dirty = false;
}

void LuaNodeEditor::Redo(int steps)
{
  while (CanRedo() && steps-- > 0)
    m_UndoBuffer[m_UndoIndex++].Redo(this);
}

bool LuaNodeEditor::CanUndo() const
{
  return m_UndoIndex > 0;
}

bool LuaNodeEditor::CanRedo() const
{
  return m_UndoIndex < (int)m_UndoBuffer.size();
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
  //int size = NodeEditor::GetSelectedObjectCount();
  //NodeEditor::NodeId* ids = nullptr;
  //int test = NodeEditor::GetSelectedNodes(ids, size);
  //for (int i = 0; i < NodeEditor::GetSelectedObjectCount(); ++i)
  //	NodeEditor::DeleteNode(ids[i]);
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

void LuaNodeEditor::UndoRecord::Undo(LuaNodeEditor* editor)
{
}

void LuaNodeEditor::UndoRecord::Redo(LuaNodeEditor* editor)
{
}