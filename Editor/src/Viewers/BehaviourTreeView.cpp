#include "BehaviourTreeView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "AI/BehaviourTree.h"
#include "AI/Decorators.h"
#include "AI/Tasks.h"
#include "AI/BehaviourTreeSerializer.h"
#include "FileSystem/FileDialog.h"
#include "ImGui/ImGuiFileEdit.h"
#include "ImGui/ImGuiVectorEdit.h"
#include "ImGui/ImGuiUtilities.h"
#include "Utilities/FileUtils.h"
#include "TinyXml2/tinyxml2.h"

#include "imgui/imgui_internal.h"

#include <algorithm>
#include <functional>
#include <unordered_map>

namespace
{
struct NodeTypeInfo
{
	BehaviourTreeView::NodeType type;
	const char* name;
	const char* tag;
	BehaviourTreeView::NodeCategory category;
	const char* icon;
	ImColor colour;
	const char* description;
};

using NodeType = BehaviourTreeView::NodeType;
using NodeCategory = BehaviourTreeView::NodeCategory;

// Selectors are blue and sequences green, with a lighter shade for the variants that resume
const ImColor c_RootColour(230, 230, 230);
const ImColor c_SelectorColour(80, 150, 255);
const ImColor c_StatefulSelectorColour(150, 190, 255);
const ImColor c_SequenceColour(70, 200, 110);
const ImColor c_MemSequenceColour(150, 230, 150);
const ImColor c_ParallelColour(240, 110, 150);
const ImColor c_DecoratorColour(190, 120, 255);
const ImColor c_TaskColour(255, 170, 60);

const NodeTypeInfo s_NodeTypes[] = {
	{ NodeType::Root, "Root", "Root", NodeCategory::Root, ICON_FA_PLAY, c_RootColour, "Entry point of the tree" },
	{ NodeType::Selector, "Selector", "Selector", NodeCategory::Composite, ICON_FA_QUESTION, c_SelectorColour, "Runs children left to right until one succeeds or is running" },
	{ NodeType::Sequence, "Sequence", "Sequence", NodeCategory::Composite, ICON_FA_ARROW_RIGHT, c_SequenceColour, "Runs children left to right until one fails or is running" },
	{ NodeType::StatefulSelector, "Stateful Selector", "StatefulSelector", NodeCategory::Composite, ICON_FA_CIRCLE_QUESTION, c_StatefulSelectorColour, "Selector that resumes from the child it last ran" },
	{ NodeType::MemSequence, "Mem Sequence", "MemSequence", NodeCategory::Composite, ICON_FA_FORWARD_STEP, c_MemSequenceColour, "Sequence that resumes from the child it last ran" },
	{ NodeType::ParallelSequence, "Parallel Sequence", "ParallelSequence", NodeCategory::Composite, ICON_FA_GRIP_LINES_VERTICAL, c_ParallelColour, "Ticks every child each update" },
	{ NodeType::BlackboardBool, "Blackboard Bool", "BlackboardBoolDecorator", NodeCategory::Decorator, ICON_FA_TOGGLE_ON, c_DecoratorColour, "Runs its child only if a blackboard bool matches" },
	{ NodeType::BlackboardCompare, "Blackboard Compare", "BlackboardCompareDecorator", NodeCategory::Decorator, ICON_FA_EQUALS, c_DecoratorColour, "Runs its child only if two blackboard bools compare as expected" },
	{ NodeType::Succeeder, "Succeeder", "SucceederDecorator", NodeCategory::Decorator, ICON_FA_CHECK, c_DecoratorColour, "Runs its child then always succeeds" },
	{ NodeType::Failer, "Failer", "FailerDecorator", NodeCategory::Decorator, ICON_FA_XMARK, c_DecoratorColour, "Runs its child then always fails" },
	{ NodeType::Inverter, "Inverter", "InverterDecorator", NodeCategory::Decorator, ICON_FA_EXCLAMATION, c_DecoratorColour, "Swaps its child's success and failure" },
	{ NodeType::Repeater, "Repeater", "RepeaterDecorator", NodeCategory::Decorator, ICON_FA_REPEAT, c_DecoratorColour, "Repeats its child forever, or up to a limit" },
	{ NodeType::UntilSuccess, "Until Success", "UntilSuccessDecorator", NodeCategory::Decorator, ICON_FA_ROTATE_RIGHT, c_DecoratorColour, "Repeats its child until it succeeds" },
	{ NodeType::UntilFailure, "Until Failure", "UntilFailureDecorator", NodeCategory::Decorator, ICON_FA_ROTATE_LEFT, c_DecoratorColour, "Repeats its child until it fails" },
	{ NodeType::Wait, "Wait", "Wait", NodeCategory::Task, ICON_FA_HOURGLASS_HALF, c_TaskColour, "Waits for a set time" },
	{ NodeType::CustomTask, "Custom Task", "CustomTask", NodeCategory::Task, ICON_FA_CODE, c_TaskColour, "Runs a Lua script's OnStateEntry, OnStateUpdate and OnStateExit" },
};

const NodeTypeInfo& GetInfo(NodeType type)
{
	for (const NodeTypeInfo& info : s_NodeTypes)
		if (info.type == type)
			return info;
	return s_NodeTypes[0];
}

const NodeTypeInfo* GetInfoFromTag(std::string_view tag)
{
	for (const NodeTypeInfo& info : s_NodeTypes)
		if (tag == info.tag)
			return &info;
	return nullptr;
}

bool HasInput(NodeType type) { return GetInfo(type).category != NodeCategory::Root; }
bool HasOutput(NodeType type) { return GetInfo(type).category != NodeCategory::Task; }
bool HasSingleChild(NodeType type) { NodeCategory category = GetInfo(type).category; return category == NodeCategory::Root || category == NodeCategory::Decorator; }

std::string GetLabel(const NodeTypeInfo& info)
{
	return std::string(info.icon) + " " + info.name;
}

using BlackboardType = BehaviourTreeView::BlackboardType;

const char* const c_BlackboardTypeNames[] = { "Bool", "Int", "Float", "Double", "String", "Vec2", "Vec3" };

const char* GetBlackboardTypeName(BlackboardType type)
{
	return c_BlackboardTypeNames[(int)type];
}

const ImColor c_WarningColour(255, 200, 80);
const ImColor c_ErrorColour(255, 90, 90);

constexpr float c_NodeWidth = 160.0f;
constexpr float c_LayoutSpacingX = 200.0f;
constexpr float c_LayoutSpacingY = 140.0f;
constexpr float c_PropertiesWidth = 260.0f;
constexpr const char* c_ClipboardTag = "BehaviourTreeClipboard";
}

/* ------------------------------------------------------------------------------------------------------------------ */

BehaviourTreeView::BehaviourTreeView(bool* show, const std::filesystem::path& filepath)
	:View("BehaviourTreeView"), m_Show(show), m_Filepath(filepath),
	m_SavePath(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath))
{
}

void BehaviourTreeView::OnAttach()
{
	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());

	NodeEditor::Config config;
	config.SettingsFile = "";
	m_NodeEditorContext = NodeEditor::CreateEditor(&config);

	Ref<BehaviourTree::BehaviourTree> behaviourTree = BehaviourTree::Serializer::Deserialize(m_SavePath);

	if (behaviourTree) {
		BuildGraph(behaviourTree);
	}
	else {
		ViewerManager::CloseViewer(m_Filepath);
	}
}

void BehaviourTreeView::OnDetach()
{
	NodeEditor::DestroyEditor(m_NodeEditorContext);
	m_NodeEditorContext = nullptr;
}

void BehaviourTreeView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (IsDirty())
		{
			ImGui::OpenPopup(("Save Prompt " + m_WindowName).c_str());
		}

		if (ImGui::BeginPopupModal(("Save Prompt " + m_WindowName).c_str()))
		{
			ImGui::TextUnformatted("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				m_SavedUndoIndex = m_UndoIndex;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*m_Show = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ViewerManager::CloseViewer(m_Filepath);
		return;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (IsDirty())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT" Undo", "Ctrl-Z", nullptr, CanUndo()))
					Undo(1);
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT" Redo", "Ctrl-Y", nullptr, CanRedo()))
					Redo(1);
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl-X", nullptr, HasSelection()))
					Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, HasSelection()))
					Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
					Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, HasSelection()))
					Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, HasSelection()))
					Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_ARROW_POINTER" Select all", "Ctrl-A", nullptr))
					SelectAll();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem(ICON_FA_EXPAND" Zoom to content"))
					m_NavigateToContent = true;
				if (ImGui::MenuItem(ICON_FA_SITEMAP" Auto layout"))
				{
					EditorState before = m_State;
					AutoLayout();
					Commit(before);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		DrawNodeEditor();
		ImGui::SameLine();
		DrawProperties();
	}

	ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::DrawNodeEditor()
{
	ImVec2 canvasSize(std::max(ImGui::GetContentRegionAvail().x - c_PropertiesWidth, 100.0f), 0.0f);
	m_CanvasMin = ImGui::GetCursorScreenPos();
	m_CanvasMax = m_CanvasMin + ImVec2(canvasSize.x, ImGui::GetContentRegionAvail().y);
	bool mouseInCanvas = ImRect(m_CanvasMin, m_CanvasMax).Contains(ImGui::GetMousePos());

	NodeEditor::SetCurrentEditor(m_NodeEditorContext);
	NodeEditor::Begin("Node Editor", canvasSize);

	if (m_PendingPaste)
	{
		std::optional<ImVec2> position;
		if (m_PasteAtMouse && mouseInCanvas)
			position = ImGui::GetMousePos();
		PasteNodes(*m_PendingPaste, position);
		m_PendingPaste.reset();
	}

	if (m_ClearSelection)
	{
		NodeEditor::ClearSelection();
		m_ClearSelection = false;
	}

	for (int nodeId : m_PendingPositions)
	{
		if (Node* node = FindNode(nodeId))
			NodeEditor::SetNodePosition(node->id, node->position);
	}
	m_PendingPositions.clear();

	NodeEditor::PushStyleVar(NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
	NodeEditor::PushStyleVar(NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));

	for (const Node& node : m_State.nodes)
		DrawNode(node);

	for (const Link& link : m_State.links)
	{
		const Node* parent = FindNode(link.parentId);
		const Node* child = FindNode(link.childId);
		if (parent && child)
			NodeEditor::Link(link.id, parent->outputPinId, child->inputPinId, GetInfo(parent->type).colour, 2.0f);
	}

	if (!m_PendingSelection.empty())
	{
		NodeEditor::ClearSelection();
		for (int nodeId : m_PendingSelection)
			NodeEditor::SelectNode(nodeId, true);
		m_PendingSelection.clear();
	}

	if (m_NavigateToContent)
	{
		NodeEditor::NavigateToContent(0.0f);
		m_NavigateToContent = false;
	}

	if (!m_CreateNewNode)
	{
		HandleCreate();
		HandleDelete();
	}

	SyncNodePositions();

	NodeEditor::PopStyleVar(2);

	ImVec2 mousePosition = ImGui::GetMousePos();

	NodeEditor::Suspend();
	NodeEditor::NodeId contextNodeId = 0;
	NodeEditor::LinkId contextLinkId = 0;
	if (NodeEditor::ShowNodeContextMenu(&contextNodeId))
	{
		m_ContextNodeId = (int)contextNodeId.Get();
		ImGui::OpenPopup("Node Context Menu");
	}
	else if (NodeEditor::ShowLinkContextMenu(&contextLinkId))
	{
		m_ContextLinkId = (int)contextLinkId.Get();
		ImGui::OpenPopup("Link Context Menu");
	}
	else if (NodeEditor::ShowBackgroundContextMenu())
	{
		m_NewNodePosition = mousePosition;
		m_NewNodeLinkPinNodeId = 0;
		ImGui::OpenPopup("Create New Node");
	}

	if (ImGui::BeginPopup("Node Context Menu"))
	{
		if (Node* node = FindNode(m_ContextNodeId))
		{
			ImGui::TextDisabled("%s", GetInfo(node->type).name);
			ImGui::Separator();
			bool isRoot = node->type == NodeType::Root;
			if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", nullptr, false, !isRoot))
				PasteNodes(SerializeNodes({ m_ContextNodeId }), std::nullopt);
			if (ImGui::MenuItem(ICON_FA_LINK_SLASH" Disconnect", nullptr, false, !isRoot))
			{
				std::vector<int> links;
				for (const Link& link : m_State.links)
					if (link.parentId == m_ContextNodeId || link.childId == m_ContextNodeId)
						links.push_back(link.id);
				RemoveNodes({}, links);
			}
			if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", nullptr, false, !isRoot))
				RemoveNodes({ m_ContextNodeId }, {});
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Link Context Menu"))
	{
		if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete"))
			RemoveNodes({}, { m_ContextLinkId });
		ImGui::EndPopup();
	}

	DrawCreateNodePopup();
	NodeEditor::Resume();

	NodeEditor::End();
	NodeEditor::SetCurrentEditor(nullptr);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::DrawNode(const Node& node)
{
	const NodeTypeInfo& info = GetInfo(node.type);
	ImColor colour = info.colour;

	std::string title = GetLabel(info);

	std::string summary;
	switch (node.type)
	{
	case NodeType::Wait:
		summary = fmt::format("{:.2f}s", node.waitTime);
		break;
	case NodeType::CustomTask:
		summary = node.scriptPath.empty() ? "No script" : node.scriptPath.filename().string();
		break;
	case NodeType::BlackboardBool:
		summary = fmt::format("{} is {}", node.key1, node.flag ? "true" : "false");
		break;
	case NodeType::BlackboardCompare:
		summary = fmt::format("{} {} {}", node.key1, node.flag ? "==" : "!=", node.key2);
		break;
	case NodeType::Repeater:
		summary = node.limit > 0 ? fmt::format("{} times", node.limit) : "Forever";
		break;
	case NodeType::Selector:			summary = "First to succeed"; break;
	case NodeType::Sequence:			summary = "All must succeed"; break;
	case NodeType::StatefulSelector:	summary = "First to succeed, resumes"; break;
	case NodeType::MemSequence:			summary = "All must succeed, resumes"; break;
	case NodeType::ParallelSequence:
		if (node.usePolicy)
			summary = fmt::format("{} succeed, {} fail", node.successOnAll ? "All" : "One", node.failOnAll ? "all" : "one");
		else
			summary = fmt::format("{} succeed, {} fail", node.minSuccess, node.minFail);
		break;
	case NodeType::Succeeder:			summary = "Always succeeds"; break;
	case NodeType::Failer:				summary = "Always fails"; break;
	case NodeType::Inverter:			summary = "Flips the result"; break;
	case NodeType::UntilSuccess:		summary = "Until it succeeds"; break;
	case NodeType::UntilFailure:		summary = "Until it fails"; break;
	default:
		break;
	}

	// Child order is shown on nodes under a composite, since it decides the order they run in
	std::string order;
	if (int parentId = GetParent(node.id))
	{
		if (const Node* parent = FindNode(parentId); parent && !HasSingleChild(parent->type))
		{
			std::vector<int> siblings = GetChildren(parentId);
			auto iter = std::find(siblings.begin(), siblings.end(), node.id);
			order = std::to_string(std::distance(siblings.begin(), iter) + 1);
		}
	}

	float width = std::max(c_NodeWidth, ImGui::CalcTextSize(title.c_str()).x + ImGui::CalcTextSize(order.c_str()).x + 16.0f);
	width = std::max(width, ImGui::CalcTextSize(summary.c_str()).x);

	NodeEditor::PushStyleColor(NodeEditor::StyleColor_NodeBorder, colour);
	NodeEditor::BeginNode(node.id);
	ImGui::PushID(node.id);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const float pinHeight = ImGui::GetTextLineHeight() * 0.6f;

	auto drawPin = [&](int pinId, NodeEditor::PinKind kind)
		{
			ImGui::Dummy(ImVec2(width, pinHeight));
			ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
			ImColor pinColour = colour;
			pinColour.Value.w = 0.5f;
			drawList->AddRectFilled(rect.Min, rect.Max, pinColour, 3.0f);
			NodeEditor::BeginPin(pinId, kind);
			NodeEditor::PinPivotRect(rect.GetTL(), rect.GetBR());
			NodeEditor::PinRect(rect.GetTL(), rect.GetBR());
			NodeEditor::EndPin();
		};

	if (HasInput(node.type))
		drawPin(node.inputPinId, NodeEditor::PinKind::Input);

	ImGui::TextUnformatted(title.c_str());
	ImRect titleRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	if (!order.empty())
	{
		float spacing = width - ImGui::CalcTextSize(title.c_str()).x - ImGui::CalcTextSize(order.c_str()).x;
		ImGui::SameLine(0.0f, std::max(spacing, ImGui::GetStyle().ItemSpacing.x));
		ImGui::TextUnformatted(order.c_str());
	}

	if (!summary.empty())
		ImGui::TextDisabled("%s", summary.c_str());

	if (HasOutput(node.type))
		drawPin(node.outputPinId, NodeEditor::PinKind::Output);

	ImGui::PopID();
	NodeEditor::EndNode();
	NodeEditor::PopStyleColor();

	// Tinted title band across the full node width, drawn behind the node contents
	if (ImDrawList* background = NodeEditor::GetNodeBackgroundDrawList(node.id))
	{
		ImVec2 nodeMin = NodeEditor::GetNodePosition(node.id);
		ImVec2 nodeMax = nodeMin + NodeEditor::GetNodeSize(node.id);
		float padding = ImGui::GetStyle().ItemSpacing.y * 0.5f;
		ImColor bandColour = colour;
		bandColour.Value.w = 0.35f;
		background->AddRectFilled(ImVec2(nodeMin.x + 1.0f, titleRect.Min.y - padding), ImVec2(nodeMax.x - 1.0f, titleRect.Max.y + padding), bandColour);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::DrawCreateNodePopup()
{
	if (ImGui::BeginPopup("Create New Node"))
	{
		m_CreateNewNode = true;

		const Node* linkNode = FindNode(m_NewNodeLinkPinNodeId);

		std::optional<NodeType> selectedType;
		NodeCategory lastCategory = NodeCategory::Root;
		for (const NodeTypeInfo& info : s_NodeTypes)
		{
			if (info.category == NodeCategory::Root)
				continue;

			// Dragged from a child's input, so the new node must be able to have children
			if (linkNode && !m_NewNodeLinkFromOutput && !HasOutput(info.type))
				continue;

			if (info.category != lastCategory)
			{
				switch (info.category)
				{
				case NodeCategory::Composite: ImGui::SeparatorText("Composites"); break;
				case NodeCategory::Decorator: ImGui::SeparatorText("Decorators"); break;
				case NodeCategory::Task: ImGui::SeparatorText("Tasks"); break;
				default: break;
				}
				lastCategory = info.category;
			}

			if (ImGui::MenuItem(GetLabel(info).c_str()))
				selectedType = info.type;
			ImGui::Tooltip(info.description);
		}

		if (selectedType)
		{
			EditorState before = m_State;
			int linkNodeId = m_NewNodeLinkPinNodeId;
			bool fromOutput = m_NewNodeLinkFromOutput;
			Node* node = SpawnNode(*selectedType, m_NewNodePosition);
			int newId = node->id;

			if (FindNode(linkNodeId))
			{
				if (fromOutput)
					AddLink(linkNodeId, newId);
				else
					AddLink(newId, linkNodeId);
			}

			Commit(before);
			m_PendingSelection = { newId };
			m_NewNodeLinkPinNodeId = 0;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	else
		m_CreateNewNode = false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::HandleCreate()
{
	if (NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor colour)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
				ImVec2 size = ImGui::CalcTextSize(label);

				ImGuiStyle& style = ImGui::GetStyle();

				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(style.ItemSpacing.x, -style.ItemSpacing.y));

				ImVec2 cursor = ImGui::GetCursorScreenPos();

				if (!std::isnan(cursor.x)) {
					ImVec2 rectMin = cursor - style.FramePadding;
					ImVec2 rectMax = cursor + size + style.FramePadding;

					ImGui::GetWindowDrawList()->AddRectFilled(rectMin, rectMax, colour, size.y * 0.15f);
					ImGui::TextUnformatted(label);
				}
			};

		NodeEditor::PinId startPinId = 0, endPinId = 0;
		if (NodeEditor::QueryNewLink(&startPinId, &endPinId))
		{
			bool startIsOutput = false, endIsOutput = false;
			Node* startNode = FindNodeByPin(startPinId, &startIsOutput);
			Node* endNode = FindNodeByPin(endPinId, &endIsOutput);

			if (startNode && endNode)
			{
				if (startIsOutput == endIsOutput)
				{
					showLabel(ICON_FA_XMARK" Incompatible pins", ImColor(45, 32, 32, 180));
					NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else
				{
					Node* parent = startIsOutput ? startNode : endNode;
					Node* child = startIsOutput ? endNode : startNode;

					if (!CanLink(parent, child))
					{
						showLabel(ICON_FA_XMARK" Would create a loop", ImColor(45, 32, 32, 180));
						NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					}
					else
					{
						showLabel(ICON_FA_PLUS" Link", ImColor(32, 45, 32, 180));
						if (NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
						{
							EditorState before = m_State;
							AddLink(parent->id, child->id);
							Commit(before);
						}
					}
				}
			}
		}

		NodeEditor::PinId pinId = 0;
		if (NodeEditor::QueryNewNode(&pinId))
		{
			bool isOutput = false;
			if (Node* node = FindNodeByPin(pinId, &isOutput))
			{
				showLabel(ICON_FA_PLUS" Create Node", ImColor(32, 45, 32, 180));

				if (NodeEditor::AcceptNewItem())
				{
					m_CreateNewNode = true;
					m_NewNodeLinkPinNodeId = node->id;
					m_NewNodeLinkFromOutput = isOutput;
					m_NewNodePosition = ImGui::GetMousePos();
					NodeEditor::Suspend();
					ImGui::OpenPopup("Create New Node");
					NodeEditor::Resume();
				}
			}
		}
	}
	NodeEditor::EndCreate();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::HandleDelete()
{
	if (NodeEditor::BeginDelete())
	{
		std::vector<int> deletedLinks;
		NodeEditor::LinkId linkId = 0;
		while (NodeEditor::QueryDeletedLink(&linkId))
		{
			if (NodeEditor::AcceptDeletedItem())
				deletedLinks.push_back((int)linkId.Get());
		}

		std::vector<int> deletedNodes;
		NodeEditor::NodeId nodeId = 0;
		while (NodeEditor::QueryDeletedNode(&nodeId))
		{
			Node* node = FindNode((int)nodeId.Get());
			if (node && node->type == NodeType::Root)
				NodeEditor::RejectDeletedItem();
			else if (NodeEditor::AcceptDeletedItem())
				deletedNodes.push_back((int)nodeId.Get());
		}

		if (!deletedLinks.empty() || !deletedNodes.empty())
			RemoveNodes(deletedNodes, deletedLinks);
	}
	NodeEditor::EndDelete();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::SyncNodePositions()
{
	// Moves are committed once the drag ends so a drag is one undo step
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		return;

	std::optional<EditorState> before;
	for (Node& node : m_State.nodes)
	{
		ImVec2 position = NodeEditor::GetNodePosition(node.id);
		if (position.x == FLT_MAX)
			continue;

		if (std::abs(position.x - node.position.x) > 0.5f || std::abs(position.y - node.position.y) > 0.5f)
		{
			if (!before)
				before = m_State;
			node.position = position;
		}
	}

	if (before)
		Commit(*before);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::DrawProperties()
{
	ImGui::BeginChild("Properties", ImVec2(0, 0), true);

	std::vector<int> selected = GetSelectedNodeIds();
	Node* node = selected.size() == 1 ? FindNode(selected[0]) : nullptr;

	if (!node)
	{
		DrawBlackboard();

		ImGui::Spacing();
		ImGui::SeparatorText("Tips");
		if (!selected.empty())
			ImGui::TextDisabled("%d nodes selected.", (int)selected.size());
		ImGui::TextDisabled("Select a node to edit it.");
		ImGui::TextDisabled("Right click the canvas to add a node.");
		ImGui::TextDisabled("Drag from a pin to link or add nodes.");
		ImGui::TextDisabled("Children run left to right.");
		ImGui::EndChild();
		return;
	}

	const NodeTypeInfo& info = GetInfo(node->type);
	ImGui::TextColored(info.colour, "%s", GetLabel(info).c_str());
	ImGui::TextWrapped("%s", info.description);
	ImGui::Separator();

	EditorState before = m_State;
	bool changed = false;
	bool finished = false;

	auto checkbox = [&](const char* label, bool& value)
		{
			if (ImGui::Checkbox(label, &value))
			{
				changed = true;
				finished = true;
			}
		};

	ImGui::PushItemWidth(-FLT_MIN);
	switch (node->type)
	{
	case NodeType::Wait:
		ImGui::TextUnformatted("Wait time (s)");
		changed |= ImGui::DragFloat("##WaitTime", &node->waitTime, 0.05f, 0.0f, FLT_MAX, "%.2f");
		finished |= ImGui::IsItemDeactivatedAfterEdit();
		break;
	case NodeType::CustomTask:
	{
		std::filesystem::path scriptPath = node->scriptPath.empty() ? std::filesystem::path() : Application::GetOpenDocumentDirectory() / node->scriptPath;
		ImGui::TextUnformatted("Script");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::FileSelect("##Script", scriptPath, FileType::SCRIPT))
		{
			node->scriptPath = FileUtils::RelativePath(scriptPath, Application::GetOpenDocumentDirectory());
			changed = true;
			finished = true;
		}
		break;
	}
	case NodeType::BlackboardBool:
		ImGui::TextUnformatted("Key");
		if (BlackboardKeyCombo("##Key", node->key1))
			changed = finished = true;
		checkbox("Is set", node->flag);
		break;
	case NodeType::BlackboardCompare:
		ImGui::TextUnformatted("Key 1");
		if (BlackboardKeyCombo("##Key1", node->key1))
			changed = finished = true;
		ImGui::TextUnformatted("Key 2");
		if (BlackboardKeyCombo("##Key2", node->key2))
			changed = finished = true;
		checkbox("Is equal", node->flag);
		break;
	case NodeType::Repeater:
		ImGui::TextUnformatted("Limit (0 repeats forever)");
		changed |= ImGui::DragInt("##Limit", &node->limit, 0.1f, 0, INT_MAX);
		finished |= ImGui::IsItemDeactivatedAfterEdit();
		break;
	case NodeType::ParallelSequence:
		checkbox("Use success/fail policy", node->usePolicy);
		if (node->usePolicy)
		{
			checkbox("Succeed only if all succeed", node->successOnAll);
			checkbox("Fail only if all fail", node->failOnAll);
		}
		else
		{
			ImGui::TextUnformatted("Minimum successes");
			changed |= ImGui::DragInt("##MinSuccess", &node->minSuccess, 0.1f, 0, INT_MAX);
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::TextUnformatted("Minimum failures");
			changed |= ImGui::DragInt("##MinFail", &node->minFail, 0.1f, 0, INT_MAX);
			finished |= ImGui::IsItemDeactivatedAfterEdit();
		}
		break;
	default:
		ImGui::TextDisabled("No properties");
		break;
	}
	ImGui::PopItemWidth();

	FinishEdit(before, changed, finished);

	ImGui::EndChild();
}

void BehaviourTreeView::FinishEdit(const EditorState& before, bool changed, bool finished)
{
	// Continuous edits (drags, typing) become one undo step when the widget is released
	if (changed && !m_EditBefore)
		m_EditBefore = before;
	if (finished && m_EditBefore)
	{
		Commit(*m_EditBefore);
		m_EditBefore.reset();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::DrawBlackboard()
{
	ImGui::SeparatorText(ICON_FA_TABLE_LIST" Blackboard");
	ImGui::TextDisabled("Starting values, shared by tasks and scripts");

	EditorState before = m_State;
	bool changed = false;
	bool finished = false;
	std::optional<size_t> removeIndex;

	const ImGuiStyle& style = ImGui::GetStyle();
	const float typeWidth = ImGui::CalcTextSize("Double").x + ImGui::GetFrameHeight() + style.FramePadding.x * 2.0f;
	const float buttonWidth = ImGui::GetFrameHeight();

	for (size_t i = 0; i < m_State.blackboard.size(); ++i)
	{
		BlackboardEntry& entry = m_State.blackboard[i];
		ImGui::PushID((int)i);

		// Renames are applied when the field is released so partial keys never reach the nodes
		bool renaming = m_RenamingEntry == (int)i;
		char keyBuffer[256];
		strncpy(keyBuffer, renaming ? m_RenameBuffer.c_str() : entry.key.c_str(), sizeof(keyBuffer) - 1);
		keyBuffer[sizeof(keyBuffer) - 1] = '\0';

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - typeWidth - buttonWidth - style.ItemSpacing.x * 2.0f);
		if (ImGui::InputText("##Key", keyBuffer, sizeof(keyBuffer)))
		{
			m_RenamingEntry = (int)i;
			m_RenameBuffer = keyBuffer;
			renaming = true;
		}
		if (ImGui::IsItemDeactivated() && renaming)
		{
			RenameBlackboardEntry(i, m_RenameBuffer);
			m_RenamingEntry = -1;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(typeWidth);
		int type = (int)entry.type;
		if (ImGui::Combo("##Type", &type, c_BlackboardTypeNames, IM_ARRAYSIZE(c_BlackboardTypeNames)))
		{
			entry.type = (BlackboardType)type;
			changed = finished = true;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_TRASH_CAN, ImVec2(buttonWidth, 0)))
			removeIndex = i;
		ImGui::Tooltip("Remove this key");

		if (renaming && m_RenameBuffer != entry.key)
		{
			if (m_RenameBuffer.empty())
				ImGui::TextColored(c_ErrorColour, "Key can't be empty");
			else if (FindBlackboardEntry(m_RenameBuffer))
				ImGui::TextColored(c_ErrorColour, "Key already exists");
		}

		ImGui::SetNextItemWidth(-FLT_MIN);
		switch (entry.type)
		{
		case BlackboardType::Bool:
			if (ImGui::Checkbox("##Value", &entry.boolValue))
				changed = finished = true;
			break;
		case BlackboardType::Int:
			changed |= ImGui::DragInt("##Value", &entry.intValue);
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		case BlackboardType::Float:
		{
			float value = (float)entry.numberValue;
			if (ImGui::DragFloat("##Value", &value, 0.1f))
			{
				entry.numberValue = value;
				changed = true;
			}
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		}
		case BlackboardType::Double:
			changed |= ImGui::DragScalar("##Value", ImGuiDataType_Double, &entry.numberValue, 0.1f);
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		case BlackboardType::String:
		{
			char buffer[256];
			strncpy(buffer, entry.stringValue.c_str(), sizeof(buffer) - 1);
			buffer[sizeof(buffer) - 1] = '\0';
			if (ImGui::InputText("##Value", buffer, sizeof(buffer)))
			{
				entry.stringValue = buffer;
				changed = true;
			}
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		}
		case BlackboardType::Vec2:
		{
			Vector2f value(entry.vectorValue.x, entry.vectorValue.y);
			if (ImGui::Vector("##Value", value))
			{
				entry.vectorValue = Vector3f(value.x, value.y, 0.0f);
				changed = true;
				// A right click reset isn't a drag, so it has no release to wait for
				finished |= !ImGui::IsAnyItemActive();
			}
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		}
		case BlackboardType::Vec3:
			if (ImGui::Vector("##Value", entry.vectorValue))
			{
				changed = true;
				finished |= !ImGui::IsAnyItemActive();
			}
			finished |= ImGui::IsItemDeactivatedAfterEdit();
			break;
		}

		ImGui::PopID();
		ImGui::Spacing();
	}

	if (m_State.blackboard.empty())
		ImGui::TextDisabled("No keys yet");

	if (ImGui::Button(ICON_FA_PLUS" Add key", ImVec2(-FLT_MIN, 0)))
	{
		BlackboardEntry entry;
		entry.key = MakeUniqueBlackboardKey("NewKey");
		m_State.blackboard.push_back(entry);
		changed = finished = true;
	}

	if (removeIndex)
	{
		m_State.blackboard.erase(m_State.blackboard.begin() + *removeIndex);
		m_RenamingEntry = -1;
		changed = finished = true;
	}

	FinishEdit(before, changed, finished);
}

bool BehaviourTreeView::BlackboardKeyCombo(const char* label, std::string& key)
{
	bool changed = false;

	if (ImGui::BeginCombo(label, key.c_str()))
	{
		bool any = false;
		for (const BlackboardEntry& entry : m_State.blackboard)
		{
			if (entry.type != BlackboardType::Bool)
				continue;
			any = true;
			if (ImGui::Selectable(entry.key.c_str(), entry.key == key))
			{
				key = entry.key;
				changed = true;
			}
		}
		if (!any)
			ImGui::TextDisabled("No bool keys in the blackboard");
		ImGui::EndCombo();
	}

	if (BlackboardEntry* entry = FindBlackboardEntry(key))
	{
		if (entry->type != BlackboardType::Bool)
			ImGui::TextColored(c_ErrorColour, ICON_FA_TRIANGLE_EXCLAMATION" '%s' is a %s, not a Bool", key.c_str(), GetBlackboardTypeName(entry->type));
	}
	else if (!key.empty())
	{
		ImGui::TextColored(c_WarningColour, ICON_FA_TRIANGLE_EXCLAMATION" Not in the blackboard");
		ImGui::Tooltip("Reads as false until a script sets it");
		ImGui::SameLine();
		if (ImGui::SmallButton((std::string(ICON_FA_PLUS" Add##") + label).c_str()))
		{
			BlackboardEntry newEntry;
			newEntry.key = key;
			m_State.blackboard.push_back(newEntry);
			changed = true;
		}
	}

	return changed;
}

BehaviourTreeView::BlackboardEntry* BehaviourTreeView::FindBlackboardEntry(const std::string& key)
{
	for (BlackboardEntry& entry : m_State.blackboard)
		if (entry.key == key)
			return &entry;
	return nullptr;
}

std::string BehaviourTreeView::MakeUniqueBlackboardKey(const std::string& base)
{
	std::string key = base;
	for (int i = 1; FindBlackboardEntry(key); ++i)
		key = base + std::to_string(i);
	return key;
}

void BehaviourTreeView::RenameBlackboardEntry(size_t index, const std::string& newKey)
{
	BlackboardEntry& entry = m_State.blackboard[index];
	if (newKey.empty() || newKey == entry.key || FindBlackboardEntry(newKey))
		return;

	EditorState before = m_State;
	std::string oldKey = entry.key;
	entry.key = newKey;

	// Keep blackboard nodes pointing at the renamed key
	if (entry.type == BlackboardType::Bool)
	{
		for (Node& node : m_State.nodes)
		{
			if (node.type != NodeType::BlackboardBool && node.type != NodeType::BlackboardCompare)
				continue;
			if (node.key1 == oldKey)
				node.key1 = newKey;
			if (node.type == NodeType::BlackboardCompare && node.key2 == oldKey)
				node.key2 = newKey;
		}
	}

	Commit(before);
}

/* ------------------------------------------------------------------------------------------------------------------ */

BehaviourTreeView::Node* BehaviourTreeView::SpawnNode(NodeType type, const ImVec2& position)
{
	Node& node = m_State.nodes.emplace_back();
	node.id = GetNextId();
	node.inputPinId = GetNextId();
	node.outputPinId = GetNextId();
	node.type = type;
	node.position = ImFloor(position);
	m_PendingPositions.push_back(node.id);
	return &node;
}

BehaviourTreeView::Node* BehaviourTreeView::FindNode(int id)
{
	for (Node& node : m_State.nodes)
		if (node.id == id)
			return &node;
	return nullptr;
}

BehaviourTreeView::Node* BehaviourTreeView::FindNodeByPin(NodeEditor::PinId pinId, bool* isOutput)
{
	int id = (int)pinId.Get();
	for (Node& node : m_State.nodes)
	{
		if (HasInput(node.type) && node.inputPinId == id)
		{
			if (isOutput) *isOutput = false;
			return &node;
		}
		if (HasOutput(node.type) && node.outputPinId == id)
		{
			if (isOutput) *isOutput = true;
			return &node;
		}
	}
	return nullptr;
}

BehaviourTreeView::Link* BehaviourTreeView::FindLinkToChild(int childId)
{
	for (Link& link : m_State.links)
		if (link.childId == childId)
			return &link;
	return nullptr;
}

std::vector<int> BehaviourTreeView::GetChildren(int parentId) const
{
	std::vector<const Node*> children;
	for (const Link& link : m_State.links)
	{
		if (link.parentId != parentId)
			continue;
		for (const Node& node : m_State.nodes)
			if (node.id == link.childId)
				children.push_back(&node);
	}

	std::stable_sort(children.begin(), children.end(), [](const Node* a, const Node* b) { return a->position.x < b->position.x; });

	std::vector<int> ids;
	for (const Node* child : children)
		ids.push_back(child->id);
	return ids;
}

int BehaviourTreeView::GetParent(int childId) const
{
	for (const Link& link : m_State.links)
		if (link.childId == childId)
			return link.parentId;
	return 0;
}

bool BehaviourTreeView::IsAncestor(int ancestorId, int nodeId) const
{
	for (int current = nodeId; current != 0; current = GetParent(current))
	{
		if (current == ancestorId)
			return true;
	}
	return false;
}

bool BehaviourTreeView::CanLink(const Node* parent, const Node* child) const
{
	if (!parent || !child || parent == child)
		return false;
	if (!HasOutput(parent->type) || !HasInput(child->type))
		return false;
	return !IsAncestor(child->id, parent->id);
}

void BehaviourTreeView::AddLink(int parentId, int childId)
{
	Node* parent = FindNode(parentId);

	// A node has one parent, and roots/decorators have one child
	m_State.links.erase(std::remove_if(m_State.links.begin(), m_State.links.end(), [&](const Link& link)
		{
			return link.childId == childId || (parent && HasSingleChild(parent->type) && link.parentId == parentId);
		}), m_State.links.end());

	m_State.links.push_back({ GetNextId(), parentId, childId });
}

void BehaviourTreeView::RemoveNodes(const std::vector<int>& nodeIds, const std::vector<int>& linkIds)
{
	EditorState before = m_State;

	auto contains = [](const std::vector<int>& ids, int id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };

	m_State.nodes.erase(std::remove_if(m_State.nodes.begin(), m_State.nodes.end(), [&](const Node& node)
		{
			return node.type != NodeType::Root && contains(nodeIds, node.id);
		}), m_State.nodes.end());
	m_State.links.erase(std::remove_if(m_State.links.begin(), m_State.links.end(), [&](const Link& link)
		{
			return contains(linkIds, link.id) || !FindNode(link.parentId) || !FindNode(link.childId);
		}), m_State.links.end());

	if (m_State.nodes.size() != before.nodes.size() || m_State.links.size() != before.links.size())
	{
		Commit(before);
		m_ClearSelection = true;
	}
}

std::vector<int> BehaviourTreeView::GetSelectedNodeIds() const
{
	NodeEditor::SetCurrentEditor(m_NodeEditorContext);
	int count = NodeEditor::GetSelectedObjectCount();
	std::vector<NodeEditor::NodeId> selected(count);
	count = NodeEditor::GetSelectedNodes(selected.data(), count);

	std::vector<int> ids;
	for (int i = 0; i < count; ++i)
		ids.push_back((int)selected[i].Get());
	return ids;
}

std::vector<int> BehaviourTreeView::GetSelectedLinkIds() const
{
	NodeEditor::SetCurrentEditor(m_NodeEditorContext);
	int count = NodeEditor::GetSelectedObjectCount();
	std::vector<NodeEditor::LinkId> selected(count);
	count = NodeEditor::GetSelectedLinks(selected.data(), count);

	std::vector<int> ids;
	for (int i = 0; i < count; ++i)
		ids.push_back((int)selected[i].Get());
	return ids;
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::string BehaviourTreeView::SerializeNodes(const std::vector<int>& nodeIds)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement(c_ClipboardTag);
	doc.InsertFirstChild(pRoot);

	auto contains = [&](int id) { return std::find(nodeIds.begin(), nodeIds.end(), id) != nodeIds.end(); };

	for (const Node& node : m_State.nodes)
	{
		if (node.type == NodeType::Root || !contains(node.id))
			continue;

		tinyxml2::XMLElement* pNode = pRoot->InsertNewChildElement("Node");
		pNode->SetAttribute("Id", node.id);
		pNode->SetAttribute("Type", GetInfo(node.type).tag);
		pNode->SetAttribute("x", node.position.x);
		pNode->SetAttribute("y", node.position.y);
		pNode->SetAttribute("WaitTime", node.waitTime);
		pNode->SetAttribute("Filepath", node.scriptPath.generic_string().c_str());
		pNode->SetAttribute("Key1", node.key1.c_str());
		pNode->SetAttribute("Key2", node.key2.c_str());
		pNode->SetAttribute("Flag", node.flag);
		pNode->SetAttribute("Limit", node.limit);
		pNode->SetAttribute("UsePolicy", node.usePolicy);
		pNode->SetAttribute("SuccessOnAll", node.successOnAll);
		pNode->SetAttribute("FailOnAll", node.failOnAll);
		pNode->SetAttribute("MinSuccess", node.minSuccess);
		pNode->SetAttribute("MinFail", node.minFail);
	}

	for (const Link& link : m_State.links)
	{
		if (!contains(link.parentId) || !contains(link.childId))
			continue;
		tinyxml2::XMLElement* pLink = pRoot->InsertNewChildElement("Link");
		pLink->SetAttribute("Parent", link.parentId);
		pLink->SetAttribute("Child", link.childId);
	}

	tinyxml2::XMLPrinter printer;
	doc.Accept(&printer);
	return printer.CStr();
}

bool BehaviourTreeView::PasteNodes(const std::string& text, std::optional<ImVec2> position)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse(text.c_str(), text.size()) != tinyxml2::XML_SUCCESS)
		return false;

	tinyxml2::XMLElement* pRoot = doc.FirstChildElement(c_ClipboardTag);
	if (!pRoot)
		return false;

	EditorState before = m_State;

	struct PastedNode { int oldId; Node node; };
	std::vector<PastedNode> pasted;
	ImVec2 minPosition(FLT_MAX, FLT_MAX);

	for (tinyxml2::XMLElement* pNode = pRoot->FirstChildElement("Node"); pNode; pNode = pNode->NextSiblingElement("Node"))
	{
		const NodeTypeInfo* info = GetInfoFromTag(pNode->Attribute("Type") ? pNode->Attribute("Type") : "");
		if (!info || info->type == NodeType::Root)
			continue;

		Node node;
		node.type = info->type;
		node.position = ImVec2(pNode->FloatAttribute("x"), pNode->FloatAttribute("y"));
		node.waitTime = pNode->FloatAttribute("WaitTime", 1.0f);
		if (const char* filepath = pNode->Attribute("Filepath"))
			node.scriptPath = filepath;
		if (const char* key1 = pNode->Attribute("Key1"))
			node.key1 = key1;
		if (const char* key2 = pNode->Attribute("Key2"))
			node.key2 = key2;
		node.flag = pNode->BoolAttribute("Flag", true);
		node.limit = pNode->IntAttribute("Limit", 0);
		node.usePolicy = pNode->BoolAttribute("UsePolicy", true);
		node.successOnAll = pNode->BoolAttribute("SuccessOnAll", true);
		node.failOnAll = pNode->BoolAttribute("FailOnAll", true);
		node.minSuccess = pNode->IntAttribute("MinSuccess", 1);
		node.minFail = pNode->IntAttribute("MinFail", 1);

		minPosition = ImMin(minPosition, node.position);
		pasted.push_back({ pNode->IntAttribute("Id"), node });
	}

	if (pasted.empty())
		return false;

	ImVec2 offset = position ? *position - minPosition : ImVec2(40.0f, 40.0f);

	std::unordered_map<int, int> idMap;
	std::vector<int> newIds;
	for (PastedNode& pastedNode : pasted)
	{
		Node* node = SpawnNode(pastedNode.node.type, pastedNode.node.position + offset);
		Node copy = pastedNode.node;
		copy.id = node->id;
		copy.inputPinId = node->inputPinId;
		copy.outputPinId = node->outputPinId;
		copy.position = node->position;
		*node = copy;
		idMap[pastedNode.oldId] = node->id;
		newIds.push_back(node->id);
	}

	for (tinyxml2::XMLElement* pLink = pRoot->FirstChildElement("Link"); pLink; pLink = pLink->NextSiblingElement("Link"))
	{
		auto parent = idMap.find(pLink->IntAttribute("Parent"));
		auto child = idMap.find(pLink->IntAttribute("Child"));
		if (parent != idMap.end() && child != idMap.end())
			AddLink(parent->second, child->second);
	}

	Commit(before);
	m_PendingSelection = newIds;
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::Copy()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	std::vector<int> selected = GetSelectedNodeIds();
	if (!selected.empty())
		ImGui::SetClipboardText(SerializeNodes(selected).c_str());
}

void BehaviourTreeView::Cut()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	Copy();
	Delete();
}

void BehaviourTreeView::Paste()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	if (const char* text = ImGui::GetClipboardText())
	{
		m_PendingPaste = text;
		m_PasteAtMouse = true;
	}
}

void BehaviourTreeView::Duplicate()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	std::vector<int> selected = GetSelectedNodeIds();
	if (!selected.empty())
	{
		m_PendingPaste = SerializeNodes(selected);
		m_PasteAtMouse = false;
	}
}

void BehaviourTreeView::Delete()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	RemoveNodes(GetSelectedNodeIds(), GetSelectedLinkIds());
}

bool BehaviourTreeView::HasSelection() const
{
	NodeEditor::SetCurrentEditor(m_NodeEditorContext);
	return NodeEditor::GetSelectedObjectCount() > 0;
}

void BehaviourTreeView::SelectAll()
{
	for (const Node& node : m_State.nodes)
		m_PendingSelection.push_back(node.id);
}

bool BehaviourTreeView::IsReadOnly() const
{
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::Save()
{
	Ref<BehaviourTree::BehaviourTree> behaviourTree = BuildBehaviourTree();
	if (BehaviourTree::Serializer::Serialize(m_SavePath, behaviourTree.get()))
		m_SavedUndoIndex = m_UndoIndex;
	else
		ENGINE_ERROR("Failed to save behaviour tree {0}", m_SavePath);
}

void BehaviourTreeView::SaveAs()
{
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", { {L"Behaviour Tree (.behaviourtree)", L"*.behaviourtree"} });

	if (dialogPath)
	{
		m_SavePath = dialogPath.value();
		if (!m_SavePath.has_extension())
			m_SavePath.replace_extension(".behaviourtree");
		Save();
	}
}

bool BehaviourTreeView::NeedsSaving()
{
	return IsDirty();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::UndoRecord::Undo(BehaviourTreeView* editor)
{
	editor->RestoreState(m_Before);
}

void BehaviourTreeView::UndoRecord::Redo(BehaviourTreeView* editor)
{
	editor->RestoreState(m_After);
}

void BehaviourTreeView::Undo(int asteps)
{
	while (CanUndo() && asteps-- > 0)
		m_UndoBuffer[--m_UndoIndex].Undo(this);
}

void BehaviourTreeView::Redo(int asteps)
{
	while (CanRedo() && asteps-- > 0)
		m_UndoBuffer[m_UndoIndex++].Redo(this);
}

bool BehaviourTreeView::CanUndo() const
{
	return m_UndoIndex > 0;
}

bool BehaviourTreeView::CanRedo() const
{
	return m_UndoIndex < (int)m_UndoBuffer.size();
}

void BehaviourTreeView::Commit(const EditorState& before)
{
	m_UndoBuffer.erase(m_UndoBuffer.begin() + m_UndoIndex, m_UndoBuffer.end());
	if (m_SavedUndoIndex > m_UndoIndex)
		m_SavedUndoIndex = -1;

	m_UndoBuffer.emplace_back(before, m_State);
	m_UndoIndex = (int)m_UndoBuffer.size();
}

void BehaviourTreeView::RestoreState(const EditorState& state)
{
	m_State = state;
	m_EditBefore.reset();
	m_RenamingEntry = -1;
	m_ClearSelection = true;
	for (const Node& node : m_State.nodes)
		m_PendingPositions.push_back(node.id);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void BehaviourTreeView::BuildGraph(Ref<BehaviourTree::BehaviourTree> behaviourTree)
{
	m_State = EditorState();

	if (Ref<BehaviourTree::Blackboard> blackboard = behaviourTree->getBlackboard())
	{
		auto addEntry = [&](const std::string& key, BlackboardType type) -> BlackboardEntry&
			{
				BlackboardEntry& entry = m_State.blackboard.emplace_back();
				entry.key = key;
				entry.type = type;
				return entry;
			};

		for (auto iter = blackboard->getBoolsBegin(); iter != blackboard->getBoolsEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Bool).boolValue = iter->second;
		for (auto iter = blackboard->getIntsBegin(); iter != blackboard->getIntsEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Int).intValue = iter->second;
		for (auto iter = blackboard->getFloatsBegin(); iter != blackboard->getFloatsEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Float).numberValue = iter->second;
		for (auto iter = blackboard->getDoublesBegin(); iter != blackboard->getDoublesEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Double).numberValue = iter->second;
		for (auto iter = blackboard->getStringsBegin(); iter != blackboard->getStringsEnd(); ++iter)
			addEntry(iter->first, BlackboardType::String).stringValue = iter->second;
		for (auto iter = blackboard->getVector2sBegin(); iter != blackboard->getVector2sEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Vec2).vectorValue = Vector3f(iter->second.x, iter->second.y, 0.0f);
		for (auto iter = blackboard->getVector3sBegin(); iter != blackboard->getVector3sEnd(); ++iter)
			addEntry(iter->first, BlackboardType::Vec3).vectorValue = iter->second;

		// The blackboard is stored in hash maps, so sort for a stable order
		std::sort(m_State.blackboard.begin(), m_State.blackboard.end(), [](const BlackboardEntry& a, const BlackboardEntry& b) { return a.key < b.key; });
	}

	Vector2f rootPosition = behaviourTree->GetEditorPosition();
	int rootId = SpawnNode(NodeType::Root, ImVec2(rootPosition.x, rootPosition.y))->id;

	if (Ref<BehaviourTree::Node> btNode = behaviourTree->getRoot())
		m_State.links.push_back({ GetNextId(), rootId, BuildGraphNode(btNode) });

	for (const Ref<BehaviourTree::Node>& btNode : behaviourTree->getUnattached())
		BuildGraphNode(btNode);

	// Files saved before positions were stored have every node at the origin
	bool hasLayout = std::any_of(m_State.nodes.begin(), m_State.nodes.end(), [](const Node& node) { return node.position.x != 0.0f || node.position.y != 0.0f; });
	if (!hasLayout)
		AutoLayout();
}

int BehaviourTreeView::BuildGraphNode(Ref<BehaviourTree::Node> btNode)
{
	namespace BT = BehaviourTree;

	NodeType type = NodeType::Root;
	if (std::dynamic_pointer_cast<BT::StatefulSelector>(btNode)) type = NodeType::StatefulSelector;
	else if (std::dynamic_pointer_cast<BT::MemSequence>(btNode)) type = NodeType::MemSequence;
	else if (std::dynamic_pointer_cast<BT::ParallelSequence>(btNode)) type = NodeType::ParallelSequence;
	else if (std::dynamic_pointer_cast<BT::Sequence>(btNode)) type = NodeType::Sequence;
	else if (std::dynamic_pointer_cast<BT::Selector>(btNode)) type = NodeType::Selector;
	else if (std::dynamic_pointer_cast<BT::BlackboardBool>(btNode)) type = NodeType::BlackboardBool;
	else if (std::dynamic_pointer_cast<BT::BlackboardCompare>(btNode)) type = NodeType::BlackboardCompare;
	else if (std::dynamic_pointer_cast<BT::Succeeder>(btNode)) type = NodeType::Succeeder;
	else if (std::dynamic_pointer_cast<BT::Failer>(btNode)) type = NodeType::Failer;
	else if (std::dynamic_pointer_cast<BT::Inverter>(btNode)) type = NodeType::Inverter;
	else if (std::dynamic_pointer_cast<BT::Repeater>(btNode)) type = NodeType::Repeater;
	else if (std::dynamic_pointer_cast<BT::UntilSuccess>(btNode)) type = NodeType::UntilSuccess;
	else if (std::dynamic_pointer_cast<BT::UntilFailure>(btNode)) type = NodeType::UntilFailure;
	else if (std::dynamic_pointer_cast<BT::Wait>(btNode)) type = NodeType::Wait;
	else if (std::dynamic_pointer_cast<BT::CustomTask>(btNode)) type = NodeType::CustomTask;
	else
	{
		ENGINE_ERROR("Unknown behaviour tree node");
		return 0;
	}

	Vector2f position = btNode->GetEditorPosition();
	Node* node = SpawnNode(type, ImVec2(position.x, position.y));
	int id = node->id;

	if (auto wait = std::dynamic_pointer_cast<BT::Wait>(btNode))
		node->waitTime = wait->getWaitTime();
	else if (auto task = std::dynamic_pointer_cast<BT::CustomTask>(btNode))
		node->scriptPath = task->getScriptPath();
	else if (auto blackboardBool = std::dynamic_pointer_cast<BT::BlackboardBool>(btNode))
	{
		node->key1 = blackboardBool->getKey();
		node->flag = blackboardBool->getIsSet();
	}
	else if (auto blackboardCompare = std::dynamic_pointer_cast<BT::BlackboardCompare>(btNode))
	{
		node->key1 = blackboardCompare->getKey1();
		node->key2 = blackboardCompare->getKey2();
		node->flag = blackboardCompare->getIsEqual();
	}
	else if (auto repeater = std::dynamic_pointer_cast<BT::Repeater>(btNode))
		node->limit = repeater->getLimit();
	else if (auto parallel = std::dynamic_pointer_cast<BT::ParallelSequence>(btNode))
	{
		node->usePolicy = parallel->usesSuccessFailPolicy();
		node->successOnAll = parallel->successOnAll();
		node->failOnAll = parallel->failOnAll();
		node->minSuccess = parallel->getMinSuccess();
		node->minFail = parallel->getMinFail();
	}

	// node is invalidated from here as children push into m_State.nodes
	if (auto composite = std::dynamic_pointer_cast<BT::Composite>(btNode))
	{
		for (const Ref<BT::Node>& child : *composite)
		{
			if (int childId = BuildGraphNode(child))
				m_State.links.push_back({ GetNextId(), id, childId });
		}
	}
	else if (auto decorator = std::dynamic_pointer_cast<BT::Decorator>(btNode))
	{
		if (decorator->hasChild())
		{
			if (int childId = BuildGraphNode(decorator->getChild()))
				m_State.links.push_back({ GetNextId(), id, childId });
		}
	}

	return id;
}

void BehaviourTreeView::AutoLayout()
{
	float nextX = 0.0f;

	std::function<float(int, int)> layout = [&](int nodeId, int depth) -> float
		{
			std::vector<int> children = GetChildren(nodeId);
			float x;
			if (children.empty())
			{
				x = nextX;
				nextX += c_LayoutSpacingX;
			}
			else
			{
				float first = 0.0f, last = 0.0f;
				for (size_t i = 0; i < children.size(); ++i)
				{
					float childX = layout(children[i], depth + 1);
					if (i == 0) first = childX;
					last = childX;
				}
				x = (first + last) * 0.5f;
			}

			if (Node* node = FindNode(nodeId))
			{
				node->position = ImVec2(x, depth * c_LayoutSpacingY);
				m_PendingPositions.push_back(nodeId);
			}
			return x;
		};

	std::vector<int> roots;
	for (const Node& node : m_State.nodes)
		if (GetParent(node.id) == 0)
			roots.push_back(node.id);

	for (int rootId : roots)
		layout(rootId, 0);

	m_NavigateToContent = true;
}

Ref<BehaviourTree::BehaviourTree> BehaviourTreeView::BuildBehaviourTree()
{
	Ref<BehaviourTree::BehaviourTree> behaviourTree = CreateRef<BehaviourTree::BehaviourTree>();

	Ref<BehaviourTree::Blackboard> blackboard = behaviourTree->getBlackboard();
	for (const BlackboardEntry& entry : m_State.blackboard)
	{
		switch (entry.type)
		{
		case BlackboardType::Bool:		blackboard->setBool(entry.key, entry.boolValue); break;
		case BlackboardType::Int:		blackboard->setInt(entry.key, entry.intValue); break;
		case BlackboardType::Float:		blackboard->setFloat(entry.key, (float)entry.numberValue); break;
		case BlackboardType::Double:	blackboard->setDouble(entry.key, entry.numberValue); break;
		case BlackboardType::String:	blackboard->setString(entry.key, entry.stringValue); break;
		case BlackboardType::Vec2:		blackboard->setVector2(entry.key, Vector2f(entry.vectorValue.x, entry.vectorValue.y)); break;
		case BlackboardType::Vec3:		blackboard->setVector3(entry.key, entry.vectorValue); break;
		}
	}

	for (const Node& node : m_State.nodes)
	{
		if (node.type == NodeType::Root)
		{
			behaviourTree->SetEditorPosition(Vector2f(node.position.x, node.position.y));
			std::vector<int> children = GetChildren(node.id);
			if (!children.empty())
				behaviourTree->setRoot(BuildBehaviourTreeNode(children.front(), behaviourTree.get()));
		}
		else if (GetParent(node.id) == 0)
		{
			if (Ref<BehaviourTree::Node> btNode = BuildBehaviourTreeNode(node.id, behaviourTree.get()))
				behaviourTree->addUnattached(btNode);
		}
	}
	return behaviourTree;
}

Ref<BehaviourTree::Node> BehaviourTreeView::BuildBehaviourTreeNode(int nodeId, BehaviourTree::BehaviourTree* behaviourTree)
{
	namespace BT = BehaviourTree;

	const Node* node = FindNode(nodeId);
	if (!node)
		return nullptr;

	Ref<BT::Node> btNode;
	switch (node->type)
	{
	case NodeType::Selector:			btNode = CreateRef<BT::Selector>(); break;
	case NodeType::Sequence:			btNode = CreateRef<BT::Sequence>(); break;
	case NodeType::StatefulSelector:	btNode = CreateRef<BT::StatefulSelector>(); break;
	case NodeType::MemSequence:			btNode = CreateRef<BT::MemSequence>(); break;
	case NodeType::ParallelSequence:
		if (node->usePolicy)
			btNode = CreateRef<BT::ParallelSequence>(node->successOnAll, node->failOnAll);
		else
			btNode = CreateRef<BT::ParallelSequence>(node->minSuccess, node->minFail);
		break;
	case NodeType::BlackboardBool:		btNode = CreateRef<BT::BlackboardBool>(behaviourTree->getBlackboard(), node->key1, node->flag); break;
	case NodeType::BlackboardCompare:	btNode = CreateRef<BT::BlackboardCompare>(behaviourTree->getBlackboard(), node->key1, node->key2, node->flag); break;
	case NodeType::Succeeder:			btNode = CreateRef<BT::Succeeder>(); break;
	case NodeType::Failer:				btNode = CreateRef<BT::Failer>(); break;
	case NodeType::Inverter:			btNode = CreateRef<BT::Inverter>(); break;
	case NodeType::Repeater:			btNode = CreateRef<BT::Repeater>(node->limit); break;
	case NodeType::UntilSuccess:		btNode = CreateRef<BT::UntilSuccess>(); break;
	case NodeType::UntilFailure:		btNode = CreateRef<BT::UntilFailure>(); break;
	case NodeType::Wait:				btNode = CreateRef<BT::Wait>(behaviourTree, node->waitTime); break;
	case NodeType::CustomTask:			btNode = CreateRef<BT::CustomTask>(behaviourTree, node->scriptPath); break;
	default: return nullptr;
	}

	btNode->SetEditorPosition(Vector2f(node->position.x, node->position.y));

	std::vector<int> children = GetChildren(nodeId);
	if (auto composite = std::dynamic_pointer_cast<BT::Composite>(btNode))
	{
		for (int childId : children)
			if (Ref<BT::Node> child = BuildBehaviourTreeNode(childId, behaviourTree))
				composite->addChild(child);
	}
	else if (auto decorator = std::dynamic_pointer_cast<BT::Decorator>(btNode))
	{
		if (!children.empty())
			decorator->setChild(BuildBehaviourTreeNode(children.front(), behaviourTree));
	}

	return btNode;
}
