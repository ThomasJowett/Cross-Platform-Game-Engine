#include "StateMachineView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "AI/StateMachine.h"
#include "AI/StateMachineSerializer.h"
#include "FileSystem/FileDialog.h"
#include "FileSystem/WebProject.h"
#include "ImGui/ImGuiFileEdit.h"
#include "ImGui/ImGuiUtilities.h"
#include "Utilities/FileUtils.h"
#include "TinyXml2/tinyxml2.h"

#include <algorithm>
#include <unordered_map>

namespace
{
const ImColor c_EntryColour(110, 210, 120);
const ImColor c_StateColour(70, 190, 210);
const ImColor c_TransitionColour(200, 210, 220);
const ImColor c_WarningColour(255, 200, 80);
const ImColor c_ErrorColour(255, 90, 90);

constexpr float c_NodeWidth = 150.0f;
constexpr float c_EntryWidth = 90.0f;
constexpr float c_NodeRounding = 18.0f;
constexpr float c_ArrowSize = 10.0f;
constexpr float c_TwoWayOffset = 8.0f; // Gap between the two arrows of a pair of states that change to each other
constexpr float c_LayoutSpacingX = 240.0f;
constexpr float c_LayoutSpacingY = 120.0f;
constexpr float c_PropertiesWidth = 260.0f;
constexpr const char* c_ClipboardTag = "StateMachineClipboard";
constexpr const char* c_DefaultStateName = "State";

std::string GetTitle(bool isEntry, const std::string& name)
{
	return isEntry ? std::string(ICON_FA_PLAY" Entry") : std::string(ICON_FA_CIRCLE_DOT" ") + name;
}

std::string GetChangeStateCall(const std::string& name)
{
	return "StateMachine.ChangeState(\"" + name + "\")";
}

// Width of a node's contents, not counting the node editor's padding
float CalcNodeContentWidth(bool isEntry, const std::string& title, const std::string& summary)
{
	float width = std::max(isEntry ? c_EntryWidth : c_NodeWidth, ImGui::CalcTextSize(title.c_str()).x);
	return std::max(width, ImGui::CalcTextSize(summary.c_str()).x);
}

void ShowCreateLabel(const char* label, ImColor colour)
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
}

// Pins that only anchor arrows still need an area to hit test, so they get a single pixel in the node's rounded corner
ImRect HiddenPinRect(const ImRect& nodeRect)
{
	return ImRect(nodeRect.Min, nodeRect.Min + ImVec2(1.0f, 1.0f));
}
}

/* ------------------------------------------------------------------------------------------------------------------ */

StateMachineView::StateMachineView(bool* show, const std::filesystem::path& filepath)
	:View("StateMachineView"), m_Show(show), m_Filepath(filepath),
	m_SavePath(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath))
{
}

void StateMachineView::OnAttach()
{
	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());

	NodeEditor::Config config;
	config.SettingsFile = "";
	m_NodeEditorContext = NodeEditor::CreateEditor(&config);

	Ref<StateMachine> stateMachine = StateMachineSerializer::Deserialize(m_SavePath);

	if (stateMachine) {
		BuildGraph(stateMachine);
	}
	else {
		ViewerManager::CloseViewer(m_Filepath);
	}
}

void StateMachineView::OnDetach()
{
	NodeEditor::DestroyEditor(m_NodeEditorContext);
	m_NodeEditorContext = nullptr;
}

void StateMachineView::OnImGuiRender()
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

void StateMachineView::DrawNodeEditor()
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

	// Zero strength makes every link a straight line between the nodes' edges
	NodeEditor::PushStyleVar(NodeEditor::StyleVar_LinkStrength, 0.0f);
	NodeEditor::PushStyleVar(NodeEditor::StyleVar_NodeRounding, c_NodeRounding);

	for (const Node& node : m_State.nodes)
		DrawNode(node);

	for (const Link& link : m_State.links)
	{
		const Node* from = FindNode(link.fromId);
		if (from && FindNode(link.toId))
			NodeEditor::Link(link.id, link.fromPinId, link.toPinId, from->isEntry ? c_EntryColour : c_TransitionColour, 2.0f);
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

	HandleCreate();
	HandleDelete();

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
		ImGui::OpenPopup("Canvas Context Menu");
	}

	if (ImGui::BeginPopup("Node Context Menu"))
	{
		if (Node* node = FindNode(m_ContextNodeId))
		{
			ImGui::TextDisabled("%s", node->isEntry ? "Entry" : node->name.c_str());
			ImGui::Separator();
			bool isEntry = node->isEntry;
			if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", nullptr, false, !isEntry))
				PasteNodes(SerializeNodes({ m_ContextNodeId }), std::nullopt);
			if (ImGui::MenuItem(ICON_FA_LINK_SLASH" Disconnect"))
			{
				std::vector<int> links;
				for (const Link& link : m_State.links)
					if (link.fromId == m_ContextNodeId || link.toId == m_ContextNodeId)
						links.push_back(link.id);
				RemoveNodes({}, links);
			}
			if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", nullptr, false, !isEntry))
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

	if (ImGui::BeginPopup("Canvas Context Menu"))
	{
		if (ImGui::MenuItem(ICON_FA_PLUS" Add State"))
		{
			EditorState before = m_State;
			Node* node = SpawnState(m_NewNodePosition, c_DefaultStateName);
			const ImVec4& padding = NodeEditor::GetStyle().NodePadding;
			node->position.x = std::floor(node->position.x - (c_NodeWidth + padding.x + padding.z) * 0.5f);
			node->position.y = std::floor(node->position.y - ImGui::GetTextLineHeight());
			m_PendingSelection = { node->id };
			Commit(before);
		}
		if (ImGui::MenuItem(ICON_FA_PASTE" Paste", nullptr, false, ImGui::GetClipboardText() != nullptr))
		{
			if (const char* text = ImGui::GetClipboardText())
				PasteNodes(text, m_NewNodePosition);
		}
		ImGui::EndPopup();
	}
	NodeEditor::Resume();

	NodeEditor::End();
	NodeEditor::SetCurrentEditor(nullptr);
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::string StateMachineView::GetNodeSummary(const Node& node) const
{
	if (node.isEntry)
	{
		const Node* initial = GetInitialState();
		return initial ? std::string() : "No initial state";
	}
	return node.scriptPath.empty() ? "No script" : node.scriptPath.filename().string();
}

void StateMachineView::DrawNode(const Node& node)
{
	const ImColor colour = node.isEntry ? c_EntryColour : c_StateColour;

	std::string title = GetTitle(node.isEntry, node.name);
	std::string summary = GetNodeSummary(node);
	bool isWarning = node.isEntry || node.scriptPath.empty();

	float width = CalcNodeContentWidth(node.isEntry, title, summary);
	ImRect nodeRect = GetNodeRect(node);

	NodeEditor::PushStyleColor(NodeEditor::StyleColor_NodeBorder, colour);
	NodeEditor::BeginNode(node.id);
	ImGui::PushID(node.id);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Title is centred, as states have no left to right order
	float titleWidth = ImGui::CalcTextSize(title.c_str()).x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (width - titleWidth) * 0.5f));
	ImGui::TextUnformatted(title.c_str());
	ImRect titleRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());


	if (!summary.empty())
	{
		if (isWarning)
			ImGui::TextColored(c_WarningColour, ICON_FA_TRIANGLE_EXCLAMATION" %s", summary.c_str());
		else
			ImGui::TextDisabled("%s", summary.c_str());
	}

	// Transitions are dragged from the bar along the bottom
	const float handleHeight = ImGui::GetTextLineHeight() * 0.6f;
	ImGui::Dummy(ImVec2(width, handleHeight));
	ImRect handleRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	ImColor handleColour = colour;
	handleColour.Value.w = 0.5f;
	drawList->AddRectFilled(handleRect.Min, handleRect.Max, handleColour, handleHeight * 0.5f);
	NodeEditor::BeginPin(node.outputPinId, NodeEditor::PinKind::Output);
	NodeEditor::PinPivotRect(nodeRect.Min, nodeRect.Max);
	NodeEditor::PinRect(handleRect.Min, handleRect.Max);
	NodeEditor::EndPin();

	// While a transition is dragged the whole state accepts it, otherwise it would block dragging the state
	if (!node.isEntry)
	{
		ImRect dropRect = m_IsCreatingLink && nodeRect.GetArea() > 0.0f ? nodeRect : HiddenPinRect(nodeRect);
		NodeEditor::BeginPin(node.inputPinId, NodeEditor::PinKind::Input);
		NodeEditor::PinPivotRect(nodeRect.Min, nodeRect.Max);
		NodeEditor::PinRect(dropRect.Min, dropRect.Max);
		NodeEditor::EndPin();
	}

	DrawLinkPins(node);

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
		background->AddRectFilled(ImVec2(nodeMin.x + 1.0f, nodeMin.y + 1.0f), ImVec2(nodeMax.x - 1.0f, titleRect.Max.y + padding), bandColour,
			c_NodeRounding - 1.0f, ImDrawFlags_RoundCornersTop);
	}
}

void StateMachineView::DrawLinkPins(const Node& node)
{
	// Every link gets its own pair of pins, so each arrow runs edge to edge between its two nodes
	for (const Link& link : m_State.links)
	{
		const bool isFrom = link.fromId == node.id;
		if (!isFrom && link.toId != node.id)
			continue;

		const Node* from = FindNode(link.fromId);
		const Node* to = FindNode(link.toId);
		if (!from || !to)
			continue;

		ImRect fromRect = GetNodeRect(*from);
		ImRect toRect = GetNodeRect(*to);

		// Two states that change to each other get side by side arrows rather than one on top of the other
		ImRect pivot = isFrom ? fromRect : toRect;
		if (FindLink(link.toId, link.fromId))
		{
			ImVec2 direction = toRect.GetCenter() - fromRect.GetCenter();
			float length = ImSqrt(direction.x * direction.x + direction.y * direction.y);
			if (length > 0.0f)
				pivot.Translate(ImVec2(-direction.y, direction.x) * (c_TwoWayOffset / length));
		}

		ImRect hidden = HiddenPinRect(GetNodeRect(node));
		if (isFrom)
		{
			NodeEditor::BeginPin(link.fromPinId, NodeEditor::PinKind::Output);
		}
		else
		{
			NodeEditor::PushStyleVar(NodeEditor::StyleVar_PinArrowSize, c_ArrowSize);
			NodeEditor::PushStyleVar(NodeEditor::StyleVar_PinArrowWidth, c_ArrowSize);
			NodeEditor::BeginPin(link.toPinId, NodeEditor::PinKind::Input);
			NodeEditor::PopStyleVar(2);
		}
		NodeEditor::PinPivotRect(pivot.Min, pivot.Max);
		NodeEditor::PinRect(hidden.Min, hidden.Max);
		NodeEditor::EndPin();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::HandleCreate()
{
	m_IsCreatingLink = false;

	if (NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		m_IsCreatingLink = true;

		NodeEditor::PinId startPinId = 0, endPinId = 0;
		if (NodeEditor::QueryNewLink(&startPinId, &endPinId))
		{
			bool startIsOutput = false, endIsOutput = false;
			Node* startNode = FindNodeByPin(startPinId, &startIsOutput);
			Node* endNode = FindNodeByPin(endPinId, &endIsOutput);

			if (startNode && endNode)
			{
				Node* from = startIsOutput ? startNode : endNode;
				Node* to = startIsOutput ? endNode : startNode;

				if (startIsOutput == endIsOutput)
				{
					ShowCreateLabel(ICON_FA_XMARK" Incompatible pins", ImColor(45, 32, 32, 180));
					NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (from == to)
				{
					ShowCreateLabel(ICON_FA_XMARK" Can't change to itself", ImColor(45, 32, 32, 180));
					NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (!CanLink(from, to))
				{
					ShowCreateLabel(ICON_FA_XMARK" Already connected", ImColor(45, 32, 32, 180));
					NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else
				{
					ShowCreateLabel(from->isEntry ? ICON_FA_PLAY" Set initial state" : ICON_FA_PLUS" Add transition", ImColor(32, 45, 32, 180));
					if (NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						EditorState before = m_State;
						AddLink(from->id, to->id);
						Commit(before);
					}
				}
			}
		}

		NodeEditor::PinId pinId = 0;
		if (NodeEditor::QueryNewNode(&pinId))
		{
			bool isOutput = false;
			Node* node = FindNodeByPin(pinId, &isOutput);
			if (node && isOutput)
			{
				ShowCreateLabel(ICON_FA_PLUS" Add state", ImColor(32, 45, 32, 180));

				if (NodeEditor::AcceptNewItem())
				{
					EditorState before = m_State;
					int fromId = node->id;
					Node* newNode = SpawnState(ImGui::GetMousePos(), c_DefaultStateName);
					int newId = newNode->id;
					newNode->position.y = std::floor(newNode->position.y - ImGui::GetTextLineHeight());
					AddLink(fromId, newId);
					Commit(before);
					m_PendingSelection = { newId };
				}
			}
		}
	}
	NodeEditor::EndCreate();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::HandleDelete()
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
			if (node && node->isEntry)
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

void StateMachineView::SyncNodePositions()
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

void StateMachineView::DrawProperties()
{
	ImGui::BeginChild("Properties", ImVec2(0, 0), true);

	std::vector<int> selectedNodes = GetSelectedNodeIds();
	std::vector<int> selectedLinks = GetSelectedLinkIds();

	Node* node = selectedNodes.size() == 1 ? FindNode(selectedNodes[0]) : nullptr;
	const Link* link = selectedNodes.empty() && selectedLinks.size() == 1 ? FindLink(selectedLinks[0]) : nullptr;

	if (node)
	{
		DrawStateProperties(*node);
	}
	else if (link)
	{
		DrawTransitionProperties(*link);
	}
	else
	{
		ImGui::SeparatorText(ICON_FA_CIRCLE_DOT" States");
		bool any = false;
		const Node* initial = GetInitialState();
		for (const Node& state : m_State.nodes)
		{
			if (state.isEntry)
				continue;
			any = true;
			std::string label = GetTitle(false, state.name) + (&state == initial ? " (initial)" : "");
			if (ImGui::Selectable((label + "##" + std::to_string(state.id)).c_str()))
				m_PendingSelection = { state.id };
		}
		if (!any)
			ImGui::TextDisabled("No states yet");

		ImGui::Spacing();
		ImGui::SeparatorText("Tips");
		if (selectedNodes.size() + selectedLinks.size() > 1)
			ImGui::TextDisabled("%d items selected.", (int)(selectedNodes.size() + selectedLinks.size()));
		ImGui::TextWrapped("Right click the canvas to add a state.");
		ImGui::TextWrapped("Drag from the bar under a state onto another state to draw a transition, or into space to add a linked state.");
		ImGui::TextWrapped("Drag from the Entry node onto a state to make it the state the machine starts in.");
		ImGui::TextWrapped("A state's script changes state by calling StateMachine.ChangeState(\"Name\"); the arrows show which changes it makes.");
	}

	ImGui::EndChild();
}

void StateMachineView::DrawStateProperties(Node& node)
{
	EditorState before = m_State;
	bool changed = false;
	bool finished = false;

	if (node.isEntry)
	{
		ImGui::TextColored(c_EntryColour, "%s", GetTitle(true, "").c_str());
		ImGui::TextWrapped("Where the state machine starts. Its arrow points at the initial state.");
		ImGui::Separator();

		const Node* initial = GetInitialState();
		ImGui::TextUnformatted("Initial state");
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginCombo("##InitialState", initial ? initial->name.c_str() : "None"))
		{
			if (ImGui::Selectable("None", !initial))
			{
				SetInitialState(0);
				changed = finished = true;
			}
			for (const Node& state : m_State.nodes)
			{
				if (state.isEntry)
					continue;
				if (ImGui::Selectable((state.name + "##" + std::to_string(state.id)).c_str(), &state == initial))
				{
					SetInitialState(state.id);
					changed = finished = true;
				}
			}
			ImGui::EndCombo();
		}
		if (!initial)
			ImGui::TextColored(c_WarningColour, ICON_FA_TRIANGLE_EXCLAMATION" Nothing runs until an initial state is set");

		FinishEdit(before, changed, finished);
		return;
	}

	ImGui::TextColored(c_StateColour, "%s", GetTitle(false, node.name).c_str());
	ImGui::TextWrapped("Runs its script's OnStateEntry when entered, OnStateUpdate every frame while active, and OnStateExit when left.");
	ImGui::Separator();

	ImGui::PushItemWidth(-FLT_MIN);

	// Renames are applied when the field is released so partial names never reach the graph
	ImGui::TextUnformatted("Name");
	bool renaming = m_RenamingNodeId == node.id;
	char nameBuffer[256];
	strncpy(nameBuffer, renaming ? m_RenameBuffer.c_str() : node.name.c_str(), sizeof(nameBuffer) - 1);
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';
	if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
	{
		m_RenamingNodeId = node.id;
		m_RenameBuffer = nameBuffer;
		renaming = true;
	}
	ImGui::Tooltip("Scripts change to this state by name, so update their ChangeState calls when renaming");
	if (ImGui::IsItemDeactivated() && renaming)
	{
		RenameState(node.id, m_RenameBuffer);
		m_RenamingNodeId = 0;
		renaming = false;
	}
	if (renaming && m_RenameBuffer != node.name)
	{
		if (m_RenameBuffer.empty())
			ImGui::TextColored(c_ErrorColour, "Name can't be empty");
		else if (std::any_of(m_State.nodes.begin(), m_State.nodes.end(), [&](const Node& other) { return !other.isEntry && other.name == m_RenameBuffer; }))
			ImGui::TextColored(c_ErrorColour, "Name already used");
	}

	// RenameState commits its own undo step
	Node* current = &node;
	before = m_State;

	ImGui::TextUnformatted("Script");
	std::filesystem::path scriptPath = current->scriptPath.empty() ? std::filesystem::path() : Application::GetOpenDocumentDirectory() / current->scriptPath;
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.x);
	if (ImGui::FileSelect("##Script", scriptPath, FileType::SCRIPT))
	{
		current->scriptPath = FileUtils::RelativePath(scriptPath, Application::GetOpenDocumentDirectory());
		changed = finished = true;
	}
	ImGui::PopItemWidth();

	ImGui::SeparatorText(ICON_FA_RIGHT_LONG" Changes to");
	bool any = false;
	for (const Link& link : m_State.links)
	{
		if (link.fromId != current->id)
			continue;
		if (const Node* to = FindNode(link.toId))
		{
			any = true;
			ImGui::PushID(link.id);
			if (ImGui::SmallButton(ICON_FA_COPY))
				ImGui::SetClipboardText(GetChangeStateCall(to->name).c_str());
			ImGui::Tooltip("Copy the ChangeState call");
			ImGui::SameLine();
			ImGui::TextUnformatted(to->name.c_str());
			ImGui::PopID();
		}
	}
	if (!any)
		ImGui::TextDisabled("No transitions drawn from this state");

	ImGui::SeparatorText(ICON_FA_RIGHT_TO_BRACKET" Entered from");
	any = false;
	for (const Link& link : m_State.links)
	{
		if (link.toId != current->id)
			continue;
		if (const Node* from = FindNode(link.fromId))
		{
			any = true;
			ImGui::BulletText("%s", from->isEntry ? "Entry" : from->name.c_str());
		}
	}
	if (!any)
		ImGui::TextDisabled("No transitions drawn to this state");

	FinishEdit(before, changed, finished);
}

void StateMachineView::DrawTransitionProperties(const Link& link)
{
	const Node* from = FindNode(link.fromId);
	const Node* to = FindNode(link.toId);
	if (!from || !to)
		return;

	if (from->isEntry)
	{
		ImGui::TextColored(c_EntryColour, ICON_FA_PLAY" Initial state");
		ImGui::TextWrapped("The state machine starts in %s.", to->name.c_str());
		return;
	}

	ImGui::TextColored(c_TransitionColour, ICON_FA_RIGHT_LONG" Transition");
	ImGui::TextWrapped("%s changes to %s when its script calls:", from->name.c_str(), to->name.c_str());

	std::string call = GetChangeStateCall(to->name);
	ImGui::TextDisabled("%s", call.c_str());
	if (ImGui::Button(ICON_FA_COPY" Copy"))
		ImGui::SetClipboardText(call.c_str());
}

void StateMachineView::FinishEdit(const EditorState& before, bool changed, bool finished)
{
	// Continuous edits become one undo step when the widget is released
	if (changed && !m_EditBefore)
		m_EditBefore = before;
	if (finished && m_EditBefore)
	{
		Commit(*m_EditBefore);
		m_EditBefore.reset();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

StateMachineView::Node* StateMachineView::SpawnState(const ImVec2& position, const std::string& name)
{
	std::string uniqueName = MakeUniqueName(name);
	Node& node = m_State.nodes.emplace_back();
	node.id = GetNextId();
	node.inputPinId = GetNextId();
	node.outputPinId = GetNextId();
	node.name = uniqueName;
	node.position = ImFloor(position);
	m_PendingPositions.push_back(node.id);
	return &node;
}

StateMachineView::Node* StateMachineView::FindNode(int id)
{
	for (Node& node : m_State.nodes)
		if (node.id == id)
			return &node;
	return nullptr;
}

const StateMachineView::Node* StateMachineView::FindNode(int id) const
{
	for (const Node& node : m_State.nodes)
		if (node.id == id)
			return &node;
	return nullptr;
}

StateMachineView::Node* StateMachineView::FindNodeByPin(NodeEditor::PinId pinId, bool* isOutput)
{
	int id = (int)pinId.Get();
	for (Node& node : m_State.nodes)
	{
		if (!node.isEntry && node.inputPinId == id)
		{
			if (isOutput) *isOutput = false;
			return &node;
		}
		if (node.outputPinId == id)
		{
			if (isOutput) *isOutput = true;
			return &node;
		}
	}
	return nullptr;
}

StateMachineView::Node* StateMachineView::GetEntry()
{
	for (Node& node : m_State.nodes)
		if (node.isEntry)
			return &node;
	return nullptr;
}

StateMachineView::Link* StateMachineView::FindLink(int id)
{
	for (Link& link : m_State.links)
		if (link.id == id)
			return &link;
	return nullptr;
}

const StateMachineView::Link* StateMachineView::FindLink(int fromId, int toId) const
{
	for (const Link& link : m_State.links)
		if (link.fromId == fromId && link.toId == toId)
			return &link;
	return nullptr;
}

const StateMachineView::Node* StateMachineView::GetInitialState() const
{
	for (const Link& link : m_State.links)
	{
		const Node* from = FindNode(link.fromId);
		if (from && from->isEntry)
			return FindNode(link.toId);
	}
	return nullptr;
}

ImRect StateMachineView::GetNodeRect(const Node& node) const
{
	// Sizes are from the last frame, which is fine as a node's size only changes when it's edited
	ImVec2 position = NodeEditor::GetNodePosition(node.id);
	if (position.x == FLT_MAX)
		position = node.position;
	return ImRect(position, position + NodeEditor::GetNodeSize(node.id));
}

std::string StateMachineView::MakeUniqueName(const std::string& base) const
{
	std::string root = base.empty() ? c_DefaultStateName : base;
	auto isUsed = [&](const std::string& name)
		{
			return std::any_of(m_State.nodes.begin(), m_State.nodes.end(), [&](const Node& node) { return !node.isEntry && node.name == name; });
		};

	std::string name = root;
	for (int i = 1; isUsed(name); ++i)
		name = root + std::to_string(i);
	return name;
}

bool StateMachineView::CanLink(const Node* from, const Node* to) const
{
	if (!from || !to || from == to || to->isEntry)
		return false;
	return !FindLink(from->id, to->id);
}

void StateMachineView::AddLink(int fromId, int toId)
{
	const Node* from = FindNode(fromId);

	// The entry points at only one state
	m_State.links.erase(std::remove_if(m_State.links.begin(), m_State.links.end(), [&](const Link& link)
		{
			return link.fromId == fromId && (link.toId == toId || (from && from->isEntry));
		}), m_State.links.end());

	Link link;
	link.id = GetNextId();
	link.fromId = fromId;
	link.toId = toId;
	link.fromPinId = GetNextId();
	link.toPinId = GetNextId();
	m_State.links.push_back(link);
}

void StateMachineView::SetInitialState(int nodeId)
{
	Node* entry = GetEntry();
	if (!entry)
		return;

	int entryId = entry->id;
	if (nodeId)
		AddLink(entryId, nodeId);
	else
		m_State.links.erase(std::remove_if(m_State.links.begin(), m_State.links.end(), [&](const Link& link) { return link.fromId == entryId; }), m_State.links.end());
}

void StateMachineView::RenameState(int nodeId, const std::string& newName)
{
	Node* node = FindNode(nodeId);
	if (!node || node->isEntry || newName.empty() || newName == node->name)
		return;
	if (std::any_of(m_State.nodes.begin(), m_State.nodes.end(), [&](const Node& other) { return !other.isEntry && other.name == newName; }))
		return;

	EditorState before = m_State;
	node->name = newName;
	Commit(before);
}

void StateMachineView::RemoveNodes(const std::vector<int>& nodeIds, const std::vector<int>& linkIds)
{
	EditorState before = m_State;

	auto contains = [](const std::vector<int>& ids, int id) { return std::find(ids.begin(), ids.end(), id) != ids.end(); };

	m_State.nodes.erase(std::remove_if(m_State.nodes.begin(), m_State.nodes.end(), [&](const Node& node)
		{
			return !node.isEntry && contains(nodeIds, node.id);
		}), m_State.nodes.end());
	m_State.links.erase(std::remove_if(m_State.links.begin(), m_State.links.end(), [&](const Link& link)
		{
			return contains(linkIds, link.id) || !FindNode(link.fromId) || !FindNode(link.toId);
		}), m_State.links.end());

	if (m_State.nodes.size() != before.nodes.size() || m_State.links.size() != before.links.size())
	{
		Commit(before);
		m_ClearSelection = true;
	}
}

std::vector<int> StateMachineView::GetSelectedNodeIds() const
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

std::vector<int> StateMachineView::GetSelectedLinkIds() const
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

std::string StateMachineView::SerializeNodes(const std::vector<int>& nodeIds)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement(c_ClipboardTag);
	doc.InsertFirstChild(pRoot);

	auto contains = [&](int id) { return std::find(nodeIds.begin(), nodeIds.end(), id) != nodeIds.end(); };

	for (const Node& node : m_State.nodes)
	{
		if (node.isEntry || !contains(node.id))
			continue;

		tinyxml2::XMLElement* pNode = pRoot->InsertNewChildElement("State");
		pNode->SetAttribute("Id", node.id);
		pNode->SetAttribute("Name", node.name.c_str());
		pNode->SetAttribute("Filepath", node.scriptPath.generic_string().c_str());
		pNode->SetAttribute("x", node.position.x);
		pNode->SetAttribute("y", node.position.y);
	}

	for (const Link& link : m_State.links)
	{
		if (!contains(link.fromId) || !contains(link.toId))
			continue;
		tinyxml2::XMLElement* pLink = pRoot->InsertNewChildElement("Transition");
		pLink->SetAttribute("From", link.fromId);
		pLink->SetAttribute("To", link.toId);
	}

	tinyxml2::XMLPrinter printer;
	doc.Accept(&printer);
	return printer.CStr();
}

bool StateMachineView::PasteNodes(const std::string& text, std::optional<ImVec2> position)
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

	for (tinyxml2::XMLElement* pNode = pRoot->FirstChildElement("State"); pNode; pNode = pNode->NextSiblingElement("State"))
	{
		Node node;
		node.position = ImVec2(pNode->FloatAttribute("x"), pNode->FloatAttribute("y"));
		if (const char* name = pNode->Attribute("Name"))
			node.name = name;
		if (const char* filepath = pNode->Attribute("Filepath"))
			node.scriptPath = filepath;

		minPosition = ImMin(minPosition, node.position);
		pasted.push_back({ pNode->IntAttribute("Id"), node });
	}

	if (pasted.empty())
		return false;

	ImVec2 offset = position ? *position - minPosition : ImVec2(40.0f, 40.0f);

	std::unordered_map<int, int> idMap;
	std::vector<int> newIds;
	for (const PastedNode& pastedNode : pasted)
	{
		Node* node = SpawnState(pastedNode.node.position + offset, pastedNode.node.name);
		node->scriptPath = pastedNode.node.scriptPath;
		idMap[pastedNode.oldId] = node->id;
		newIds.push_back(node->id);
	}

	for (tinyxml2::XMLElement* pLink = pRoot->FirstChildElement("Transition"); pLink; pLink = pLink->NextSiblingElement("Transition"))
	{
		auto from = idMap.find(pLink->IntAttribute("From"));
		auto to = idMap.find(pLink->IntAttribute("To"));
		if (from != idMap.end() && to != idMap.end())
			AddLink(from->second, to->second);
	}

	Commit(before);
	m_PendingSelection = newIds;
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::Copy()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	std::vector<int> selected = GetSelectedNodeIds();
	if (!selected.empty())
		ImGui::SetClipboardText(SerializeNodes(selected).c_str());
}

void StateMachineView::Cut()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	Copy();
	Delete();
}

void StateMachineView::Paste()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	if (const char* text = ImGui::GetClipboardText())
	{
		m_PendingPaste = text;
		m_PasteAtMouse = true;
	}
}

void StateMachineView::Duplicate()
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

void StateMachineView::Delete()
{
	if (ImGui::GetIO().WantTextInput)
		return;

	RemoveNodes(GetSelectedNodeIds(), GetSelectedLinkIds());
}

bool StateMachineView::HasSelection() const
{
	NodeEditor::SetCurrentEditor(m_NodeEditorContext);
	return NodeEditor::GetSelectedObjectCount() > 0;
}

void StateMachineView::SelectAll()
{
	for (const Node& node : m_State.nodes)
		m_PendingSelection.push_back(node.id);
}

bool StateMachineView::IsReadOnly() const
{
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::Save()
{
	Ref<StateMachine> stateMachine = BuildStateMachine();
	if (StateMachineSerializer::Serialize(m_SavePath, stateMachine.get()))
		m_SavedUndoIndex = m_UndoIndex;
	else
		ENGINE_ERROR("Failed to save state machine {0}", m_SavePath);

	WebProject::Sync();
}

void StateMachineView::SaveAs()
{
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", { {L"State Machine (.statemachine)", L"*.statemachine"} });

	if (dialogPath)
	{
		m_SavePath = dialogPath.value();
		if (!m_SavePath.has_extension())
			m_SavePath.replace_extension(".statemachine");
		Save();
	}
}

bool StateMachineView::NeedsSaving()
{
	return IsDirty();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::UndoRecord::Undo(StateMachineView* editor)
{
	editor->RestoreState(m_Before);
}

void StateMachineView::UndoRecord::Redo(StateMachineView* editor)
{
	editor->RestoreState(m_After);
}

void StateMachineView::Undo(int asteps)
{
	while (CanUndo() && asteps-- > 0)
		m_UndoBuffer[--m_UndoIndex].Undo(this);
}

void StateMachineView::Redo(int asteps)
{
	while (CanRedo() && asteps-- > 0)
		m_UndoBuffer[m_UndoIndex++].Redo(this);
}

bool StateMachineView::CanUndo() const
{
	return m_UndoIndex > 0;
}

bool StateMachineView::CanRedo() const
{
	return m_UndoIndex < (int)m_UndoBuffer.size();
}

void StateMachineView::Commit(const EditorState& before)
{
	m_UndoBuffer.erase(m_UndoBuffer.begin() + m_UndoIndex, m_UndoBuffer.end());
	if (m_SavedUndoIndex > m_UndoIndex)
		m_SavedUndoIndex = -1;

	m_UndoBuffer.emplace_back(before, m_State);
	m_UndoIndex = (int)m_UndoBuffer.size();
}

void StateMachineView::RestoreState(const EditorState& state)
{
	m_State = state;
	m_EditBefore.reset();
	m_RenamingNodeId = 0;
	m_ClearSelection = true;
	for (const Node& node : m_State.nodes)
		m_PendingPositions.push_back(node.id);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachineView::BuildGraph(Ref<StateMachine> stateMachine)
{
	m_State = EditorState();

	Vector2f entryPosition = stateMachine->GetEditorPosition();
	Node& entry = m_State.nodes.emplace_back();
	entry.id = GetNextId();
	entry.outputPinId = GetNextId();
	entry.isEntry = true;
	entry.position = ImVec2(entryPosition.x, entryPosition.y);
	int entryId = entry.id;
	m_PendingPositions.push_back(entryId);

	std::unordered_map<std::string, int> ids;
	for (const Ref<State>& state : stateMachine->GetStates())
	{
		Vector2f position = state->GetEditorPosition();
		Node* node = SpawnState(ImVec2(position.x, position.y), state->GetName());
		if (Ref<LuaState> luaState = std::dynamic_pointer_cast<LuaState>(state))
			node->scriptPath = luaState->GetScriptPath();
		ids[node->name] = node->id;
	}

	for (const StateMachine::Transition& transition : stateMachine->GetTransitions())
	{
		auto from = ids.find(transition.from);
		auto to = ids.find(transition.to);
		if (from != ids.end() && to != ids.end() && from->second != to->second)
			AddLink(from->second, to->second);
	}

	if (auto initial = ids.find(stateMachine->GetInitialState()); initial != ids.end())
		AddLink(entryId, initial->second);

	// Hand written files may leave out positions, leaving every node at the origin
	bool hasLayout = std::any_of(m_State.nodes.begin(), m_State.nodes.end(), [](const Node& node) { return node.position.x != 0.0f || node.position.y != 0.0f; });
	if (!hasLayout)
		AutoLayout();
}

void StateMachineView::AutoLayout()
{
	// Columns by how many transitions each state is from the entry, unreachable states in a column at the end
	std::unordered_map<int, int> depths;
	std::vector<int> queue;
	if (Node* entry = GetEntry())
	{
		depths[entry->id] = 0;
		queue.push_back(entry->id);
	}

	for (size_t i = 0; i < queue.size(); ++i)
	{
		int nodeId = queue[i];
		for (const Link& link : m_State.links)
		{
			if (link.fromId == nodeId && depths.find(link.toId) == depths.end())
			{
				depths[link.toId] = depths[nodeId] + 1;
				queue.push_back(link.toId);
			}
		}
	}

	int maxDepth = 0;
	for (const auto& [id, depth] : depths)
		maxDepth = std::max(maxDepth, depth);

	std::unordered_map<int, int> rows;
	for (Node& node : m_State.nodes)
	{
		auto found = depths.find(node.id);
		int depth = found != depths.end() ? found->second : maxDepth + 1;
		int row = rows[depth]++;
		node.position = ImVec2(depth * c_LayoutSpacingX, row * c_LayoutSpacingY);
		m_PendingPositions.push_back(node.id);
	}

	m_NavigateToContent = true;
}

Ref<StateMachine> StateMachineView::BuildStateMachine() const
{
	Ref<StateMachine> stateMachine = CreateRef<StateMachine>();

	for (const Node& node : m_State.nodes)
	{
		if (node.isEntry)
		{
			stateMachine->SetEditorPosition(Vector2f(node.position.x, node.position.y));
			continue;
		}

		Ref<LuaState> state = CreateRef<LuaState>(node.name, node.scriptPath);
		state->SetEditorPosition(Vector2f(node.position.x, node.position.y));
		stateMachine->AddState(state);
	}

	for (const Link& link : m_State.links)
	{
		const Node* from = FindNode(link.fromId);
		const Node* to = FindNode(link.toId);
		if (from && to && !from->isEntry)
			stateMachine->AddTransition(from->name, to->name);
	}

	if (const Node* initial = GetInitialState())
		stateMachine->SetInitialState(initial->name);

	return stateMachine;
}
