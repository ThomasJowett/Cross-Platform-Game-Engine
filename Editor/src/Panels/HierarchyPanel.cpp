#include "HierarchyPanel.h"

#include "imgui/imgui.h"
#include "IconsFontAwesome6.h"
#include "History/HistoryCommands.h"

#include "Engine.h"

#include "MainDockSpace.h"

HierarchyPanel::HierarchyPanel(bool* show)
	:Layer("Hierarchy"), m_Show(show)
{
	m_TextFilter = new ImGuiTextFilter();
}

HierarchyPanel::~HierarchyPanel()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::OnAttach()
{
}

void HierarchyPanel::OnDetach()
{
	m_TextFilter->Clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::OnFixedUpdate()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		m_TextFilter->Clear();
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_SITEMAP" Hierarchy", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) || !m_SelectedEntity.BelongsToScene(SceneManager::CurrentScene()))
		{
			m_SelectedEntity = {};
		}

		// right click on a blank space
		if (ImGui::BeginPopupContextWindow())
		{
			m_SelectedEntity = {};
			CreateMenu();
			ImGui::EndPopup();
		}

		ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
		ImGui::SameLine();
		m_TextFilter->Draw("##Search", ImGui::GetContentRegionAvail().x);
		ImGui::Tooltip("Filter (\"incl,-excl\")");

		if (SceneManager::IsSceneLoaded())
		{
			if (m_SelectedEntity && !m_SelectedEntity.IsSceneValid())
				m_SelectedEntity = Entity();

			if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_SpanAvailWidth
				| ImGuiTreeNodeFlags_Bullet
				| ImGuiTreeNodeFlags_OpenOnDoubleClick))
			{
				DragDropTarget(Entity());

				SceneManager::CurrentScene()->GetRegistry().each([&](auto entityID)
					{
						if (SceneManager::CurrentScene()->GetRegistry().valid(entityID))
						{
							Entity entity{ entityID, SceneManager::CurrentScene() };

							if (entity.IsSceneValid() && entity.HasComponent<PrimitiveComponent>())
							{
								PrimitiveComponent& primitiveComp = entity.GetComponent<PrimitiveComponent>();

								if (primitiveComp.needsUpdating)
								{
									primitiveComp.SetType(primitiveComp.type);
								}
							}
							// Only draw a node for root entities, children are drawn recursively
							HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();
							if (!hierarchyComp || hierarchyComp->parent == entt::null
								|| (m_TextFilter->IsActive() && m_TextFilter->PassFilter(entity.GetName().c_str())))
								DrawNode(entity);
						}
					});
				ImGui::TreePop();
			}

			ImVec2 available = ImGui::GetContentRegionAvail();
			ImGui::Dummy(available);
			DragDropTarget(Entity());
		}
	}
	ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<AppOpenDocumentChangedEvent>([=](AppOpenDocumentChangedEvent&)
		{
			// Clear the selected entity when project changes
			m_SelectedEntity = Entity();
			return false;
		});
}

/* ------------------------------------------------------------------------------------------------------------------ */

Entity HierarchyPanel::GetSelectedEntity()
{
	return m_SelectedEntity;
}

void HierarchyPanel::SetSelectedEntity(Entity entity)
{
	m_SelectedEntity = entity;
}

void HierarchyPanel::DrawNode(Entity entity)
{
	std::string& name = entity.GetName();

	if (m_TextFilter->IsActive() && !m_TextFilter->PassFilter(name.c_str()))
		return;

	bool hasChildren = false;
	HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();

	if (hierarchyComp != nullptr && hierarchyComp->firstChild != entt::null)
		hasChildren = true;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen
		| ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| (!hasChildren || m_TextFilter->IsActive() ? ImGuiTreeNodeFlags_Leaf : 0);

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
		ImGui::TextUnformatted(name.c_str());
		ImGui::EndDragDropSource();
	}
	DragDropTarget(entity);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right) || ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_SelectedEntity = entity;
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			m_Focused = true;
		}
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem(std::string(name.c_str() + std::to_string((uint32_t)entity)).c_str()))
	{
		m_SelectedEntity = entity;
		if (ImGui::BeginMenu("Create")) {
			CreateMenu();
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl + X", nullptr, HasSelection()))
			Cut();
		if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, HasSelection()))
			Copy();
		if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, ImGui::GetClipboardText() != nullptr))
			Paste();
		if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, HasSelection()))
			Duplicate();
		if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, HasSelection()))
		{
			entityDeleted = true;
		}
		ImGui::EndPopup();
	}

	if (opened)
	{
		if (hasChildren && !m_TextFilter->IsActive())
		{
			entt::entity child = entity.TryGetComponent<HierarchyComponent>()->firstChild;
			while (child != entt::null)
			{
				Entity childEntity = { child, SceneManager::CurrentScene() };

				DrawNode(childEntity);

				if (childEntity.IsSceneValid())
				{
					HierarchyComponent* childHierarchyComp = childEntity.TryGetComponent<HierarchyComponent>();

					ASSERT(childHierarchyComp != nullptr, "Child does not have a Hierarchy Component!");

					child = childHierarchyComp->nextSibling;
				}
				else
				{
					child = entt::null;
				}
			}
		}

		ImGui::TreePop();
	}

	if (entityDeleted)
	{
		if (m_SelectedEntity == entity)
			m_SelectedEntity = {};
		SceneManager::CurrentScene()->RemoveEntity(entity);
	}
}

void HierarchyPanel::DragDropTarget(Entity parent)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_None))
		{
			Entity* childEntity = (Entity*)payload->Data;
			ASSERT(payload->DataSize == sizeof(Entity), "Drag-drop entity data not the correct size");

			Ref<ReparentEntityCommand> reparentCommand = CreateRef<ReparentEntityCommand>(*childEntity, parent);

			if (parent)
				SceneGraph::Reparent(*childEntity, parent);
			else
				SceneGraph::Unparent(*childEntity);

			HistoryManager::AddHistoryRecord(reparentCommand);
			SceneManager::CurrentScene()->MakeDirty();
		}
		ImGui::EndDragDropTarget();
	}
}

void HierarchyPanel::CreateMenu()
{
	if (ImGui::MenuItem("Create Empty Entity"))
	{
		Entity entity = SceneManager::CurrentScene()->CreateEntity("New Entity");//TODO: make all the entities have a unique name
		HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
		if (m_SelectedEntity) {
			m_SelectedEntity.AddChild(entity);
		}
		m_SelectedEntity = entity;
	}
	if (ImGui::MenuItem("Create World entity"))
	{
		Entity entity = SceneManager::CurrentScene()->CreateEntity("New Entity");
		entity.AddComponent<TransformComponent>();
		HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
		if (m_SelectedEntity) {
			m_SelectedEntity.AddChild(entity);
		}
		m_SelectedEntity = entity;
	}
	if (ImGui::BeginMenu("3D Object"))
	{
		if (ImGui::MenuItem("Cube"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Cube");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cube);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Sphere"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Sphere");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Sphere);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Plane"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Plane");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Plane);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Cylinder"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Cylinder");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cylinder);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Cone"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Cone");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cone);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Torus"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Torus");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Torus);
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Terrain", "", nullptr, false))
		{
			//TODO: Create terrain
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("2D Object"))
	{
		if (ImGui::MenuItem("Sprite"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Sprite");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<SpriteComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Animated Sprite"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Animated Sprite");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<AnimatedSpriteComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Circle"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Circle");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<CircleRendererComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Tilemap"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Tilemap");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<TilemapComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Audio"))
	{
		if (ImGui::MenuItem("Audio Source"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Audio Source");
			entity.AddComponent<AudioSourceComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Audio Listener", "", nullptr, false))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Audio Listener");
			entity.AddComponent<AudioListenerComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity)
				m_SelectedEntity.AddChild(entity);
			m_SelectedEntity = entity;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Light"))
	{
		if (ImGui::MenuItem("Point Light"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Point Light");
			entity.AddComponent<TransformComponent>();
			entity.AddComponent<PointLightComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Directional Light", "", nullptr, false))
		{
			//TODO: Create Directional light
		}

		if (ImGui::MenuItem("Spot Light", "", nullptr, false))
		{
			//TODO: Create Spot light
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Particle Emitter", "", nullptr, false))
	{
		//TODO: create a particle emitter component
	}
	if (ImGui::MenuItem("Camera"))
	{
		Entity entity = SceneManager::CurrentScene()->CreateEntity("Camera");
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<CameraComponent>();
		HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
		if (m_SelectedEntity) {
			m_SelectedEntity.AddChild(entity);
		}
		m_SelectedEntity = entity;
	}
	if (ImGui::BeginMenu("UI"))
	{
		if (ImGui::MenuItem("Canvas"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Canvas");
			entity.AddComponent<CanvasComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		if (ImGui::MenuItem("Button"))
		{
			Entity entity = SceneManager::CurrentScene()->CreateEntity("Button");
			entity.AddComponent<WidgetComponent>();
			entity.AddComponent<ButtonComponent>();
			HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(entity));
			if (m_SelectedEntity) {
				m_SelectedEntity.AddChild(entity);
			}
			m_SelectedEntity = entity;
		}
		ImGui::EndMenu();
	}
}

void HierarchyPanel::Undo(int asteps)
{
	HistoryManager::Undo(asteps);
}

void HierarchyPanel::Redo(int asteps)
{
	HistoryManager::Redo(asteps);
}

bool HierarchyPanel::CanUndo() const
{
	return HistoryManager::CanUndo();
}

bool HierarchyPanel::CanRedo() const
{
	return HistoryManager::CanRedo();
}

void HierarchyPanel::Copy()
{
	CLIENT_DEBUG("Copied");
	ImGui::SetClipboardText(SceneSerializer::SerializeEntity(m_SelectedEntity).c_str());
}

void HierarchyPanel::Cut()
{
	Copy();
	Delete();
}

void HierarchyPanel::Paste()
{
	m_SelectedEntity = SceneSerializer::DeserializeEntity(SceneManager::CurrentScene(), ImGui::GetClipboardText(), true);
	if(m_SelectedEntity)
		HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(m_SelectedEntity));
}

void HierarchyPanel::Duplicate()
{
	CLIENT_DEBUG("Duplicated");
	m_SelectedEntity = SceneManager::CurrentScene()->DuplicateEntity(m_SelectedEntity, Entity());
	m_SelectedEntity.GetName() += "_copy";
	HistoryManager::AddHistoryRecord(CreateRef<AddEntityCommand>(m_SelectedEntity));
}

void HierarchyPanel::Delete()
{
	HistoryManager::AddHistoryRecord(CreateRef<RemoveEntityCommand>(m_SelectedEntity));
	m_SelectedEntity.Destroy();
	m_SelectedEntity = {};
}

bool HierarchyPanel::HasSelection() const
{
	return m_SelectedEntity;
}

void HierarchyPanel::SelectAll()
{
	CLIENT_ERROR("Multi select entities not implemented yet!");
}

bool HierarchyPanel::IsReadOnly() const
{
	return false;
}