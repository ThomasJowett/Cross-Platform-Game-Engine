#include "BehaviourTreeView.h"

#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "AI/BehaviorTree.h"
#include "AI/Decorators.h"
#include "AI/Tasks.h"
#include "AI/BehaviourTreeSerializer.h"

#include "imgui/imgui_internal.h"

ImVec2 ToImVec2(Vector2f vec) { return ImVec2(vec.x, vec.y); }

ImRect ImGui_GetItemRect() { return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()); }

BehaviourTreeView::BehaviourTreeView(bool* show, const std::filesystem::path& filepath)
	:View("BehaviourTreeView"), m_Show(show), m_Filepath(filepath)
{
}

void BehaviourTreeView::OnAttach()
{
	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());

	NodeEditor::Config config;
	m_NodeEditorContext = NodeEditor::CreateEditor(&config);

	m_BehaviourTree = BehaviourTree::Serializer::Deserialize(m_Filepath);
	if (m_BehaviourTree)
		m_LocalBehaviourTree = CreateRef<BehaviourTree::BehaviourTree>(*m_BehaviourTree);
	else
		ViewerManager::CloseViewer(m_Filepath);

	BuildNodeList();
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
		if (m_Dirty)
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
				m_Dirty = false;
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

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
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
			ImGui::EndMenuBar();
		}

		NodeEditor::SetCurrentEditor(m_NodeEditorContext);
		NodeEditor::Begin("Node Editor");
		NodeEditor::PushStyleVar(NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f)); 
		NodeEditor::PushStyleVar(NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
		for (auto& node : m_Nodes)
		{
			NodeEditor::BeginNode(node.id);
			if(node.btNodeRef)
				NodeEditor::SetNodePosition(node.id, ToImVec2(node.btNodeRef->GetEditorPosition()));
			if (node.input) {
				ImRect inputRect;
				ImGui::Dummy(ImVec2(64.0f, ImGui::GetTextLineHeightWithSpacing()));
				inputRect = ImGui_GetItemRect();
				NodeEditor::BeginPin(node.input->id, NodeEditor::PinKind::Input);
				NodeEditor::PinPivotRect(inputRect.GetTL(), inputRect.GetBR());
				NodeEditor::PinRect(inputRect.GetTL(), inputRect.GetBR());
				NodeEditor::EndPin();
			}
			ImGui::TextUnformatted(node.name.c_str());

			for (auto& decorator : node.decorators)
			{
				ImGui::TextUnformatted(decorator.c_str());
			}

			if (node.output) {
				ImRect outputRect;
				ImGui::Dummy(ImVec2(64.0f, ImGui::GetTextLineHeightWithSpacing()));
				outputRect = ImGui_GetItemRect();
				NodeEditor::BeginPin(node.output->id, NodeEditor::PinKind::Output);
				NodeEditor::PinPivotRect(outputRect.GetTL(), outputRect.GetBR());
				NodeEditor::PinRect(outputRect.GetTL(), outputRect.GetBR());
				NodeEditor::EndPin();
			}
			NodeEditor::EndNode();
		}

		for (auto& link : m_Links)
		{
			NodeEditor::Link(link.id, link.startPinId, link.endPinId);
		}

		//DrawNode(m_LocalBehaviourTree->getRoot());
		NodeEditor::PopStyleVar(2);
		NodeEditor::End();
		NodeEditor::SetCurrentEditor(nullptr);
	}

	ImGui::End();
}

void BehaviourTreeView::Copy()
{
}

void BehaviourTreeView::Cut()
{
}

void BehaviourTreeView::Paste()
{
}

void BehaviourTreeView::Duplicate()
{
}

void BehaviourTreeView::Delete()
{
}

bool BehaviourTreeView::HasSelection() const
{
	return false;
}

void BehaviourTreeView::SelectAll()
{
}

bool BehaviourTreeView::IsReadOnly() const
{
	return false;
}

void BehaviourTreeView::Save()
{
	BehaviourTree::Serializer::Serialize(m_Filepath, m_LocalBehaviourTree.get());
	m_BehaviourTree = BehaviourTree::Serializer::Deserialize(m_Filepath);
	m_LocalBehaviourTree = CreateRef<BehaviourTree::BehaviourTree>(*m_BehaviourTree);
	m_Dirty = false;
}

void BehaviourTreeView::SaveAs()
{
}

bool BehaviourTreeView::NeedsSaving()
{
	return false;
}

void BehaviourTreeView::Undo(int asteps)
{
}

void BehaviourTreeView::Redo(int asteps)
{
}

bool BehaviourTreeView::CanUndo() const
{
	return false;
}

bool BehaviourTreeView::CanRedo() const
{
	return false;
}


void BehaviourTreeView::DrawNode(Ref<BehaviourTree::Node> node)
{
	
}

void BehaviourTreeView::BuildNodeList()
{
	m_Nodes.emplace_back(GetNextId(), "Root", nullptr);
	m_Nodes[0].output = CreateRef<Pin>(GetNextId(), &m_Nodes[0], PinKind::Output);
	Ref<BehaviourTree::Node> btNode = m_LocalBehaviourTree->getRoot();

	auto node = BuildNode(btNode);

	m_Links.emplace_back(Link(GetNextId(), node->input->id, m_Nodes[0].output->id));
}

BehaviourTreeView::Node* BehaviourTreeView::BuildNode(Ref<BehaviourTree::Node> btNode)
{
	auto CreateCompositeNode = [&](Ref<BehaviourTree::Composite> composite, const std::string_view name)
	{
		m_Nodes.emplace_back(GetNextId(), name, btNode);
		size_t pos = m_Nodes.size() - 1;
		m_Nodes[pos].input = CreateRef<Pin>(GetNextId(), &m_Nodes[pos], PinKind::Input);
		m_Nodes[pos].output = CreateRef<Pin>(GetNextId(), &m_Nodes[pos], PinKind::Output);

		for (auto& child : *composite)
		{
			auto childNode = BuildNode(child);
			m_Links.emplace_back(Link(GetNextId(), m_Nodes[pos].output->id, childNode->input->id));
		}
		return &m_Nodes[pos];
	};

	auto CreateDecorator = [&](Ref<BehaviourTree::Decorator> decorator, const std::string_view name)
	{
		auto node = BuildNode(decorator->getChild());
		node->decorators.emplace_back(name);
		return node;
	};

	// Composites -----------------------------------------
	if (auto composite = std::dynamic_pointer_cast<BehaviourTree::StatefulSelector>(btNode)) {
		return CreateCompositeNode(composite, "Stateful Selector");
	}
	else if (auto composite = std::dynamic_pointer_cast<BehaviourTree::MemSequence>(btNode)) {
		return CreateCompositeNode(composite, "Mem Sequence");
	}
	else if (auto composite = std::dynamic_pointer_cast<BehaviourTree::ParallelSequence>(btNode)) {
		return CreateCompositeNode(composite, "Parallel Sequence");
	}
	else if (auto composite = std::dynamic_pointer_cast<BehaviourTree::Sequence>(btNode)) {
		return CreateCompositeNode(composite, "Sequence");
	}
	else if (auto composite = std::dynamic_pointer_cast<BehaviourTree::Selector>(btNode)) {
		return CreateCompositeNode(composite, "Selector");
	}

	// Decorators -----------------------------------------
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::BlackboardBool>(btNode)) {
		return CreateDecorator(decorator, "Blackboard Bool");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::BlackboardCompare>(btNode)) {
		return CreateDecorator(decorator, "Blackboard Compare");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::Succeeder>(btNode)) {
		return CreateDecorator(decorator, "Succeeder");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::Failer>(btNode)) {
		return CreateDecorator(decorator, "Failer");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::Inverter>(btNode)) {
		return CreateDecorator(decorator, "Inverter");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::Repeater>(btNode)) {
		return CreateDecorator(decorator, "Repeater");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::UntilSuccess>(btNode)) {
		return CreateDecorator(decorator, "Until Success");
	}
	else if (auto decorator = std::dynamic_pointer_cast<BehaviourTree::UntilFailure>(btNode)) {
		return CreateDecorator(decorator, "Until Failure");
	}

	// Tasks -----------------------------------------
	else if (auto task = std::dynamic_pointer_cast<BehaviourTree::Wait>(btNode)) {
		m_Nodes.emplace_back(GetNextId(), "Wait", btNode);
		size_t pos = m_Nodes.size() - 1;
		m_Nodes[pos].input = CreateRef<Pin>(GetNextId(), &(m_Nodes[pos]), PinKind::Input);
		m_Nodes[pos].output = nullptr;
		return &m_Nodes[pos];
	}
	else if (auto task = std::dynamic_pointer_cast<BehaviourTree::CustomTask>(btNode)) {
		m_Nodes.emplace_back(GetNextId(), task->getFilePath().filename().string(), btNode);
		size_t pos = m_Nodes.size() - 1;
		m_Nodes[pos].input = CreateRef<Pin>(GetNextId(), &(m_Nodes[pos]), PinKind::Input);
		m_Nodes[pos].output = nullptr;
		return &m_Nodes[pos];
	}

	ENGINE_ERROR("Unknown behaviour tree node!");
	return nullptr;
}
