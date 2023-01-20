#include "HierarchyPanel.h"

#include "imgui/imgui.h"
#include "IconsFontAwesome6.h"

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

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() || !m_SelectedEntity.BelongsToScene(SceneManager::CurrentScene()))
		{
			m_SelectedEntity = {};
		}

		// right click on a blank space
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_SelectedEntity = SceneManager::CurrentScene()->CreateEntity("New Entity");//TODO: make all the entities have a unique name
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					Entity cubeEntity = SceneManager::CurrentScene()->CreateEntity("Cube");
					cubeEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cube);
					m_SelectedEntity = cubeEntity;
				}
				if (ImGui::MenuItem("Sphere"))
				{
					Entity sphereEntity = SceneManager::CurrentScene()->CreateEntity("Sphere");
					sphereEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Sphere);
					m_SelectedEntity = sphereEntity;
				}
				if (ImGui::MenuItem("Plane"))
				{
					Entity planeEntity = SceneManager::CurrentScene()->CreateEntity("Plane");
					planeEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Plane);
					m_SelectedEntity = planeEntity;
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					Entity cylinderEntity = SceneManager::CurrentScene()->CreateEntity("Cylinder");
					cylinderEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cylinder);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Cone"))
				{
					Entity cylinderEntity = SceneManager::CurrentScene()->CreateEntity("Cone");
					cylinderEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cone);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Torus"))
				{
					Entity torusEntity = SceneManager::CurrentScene()->CreateEntity("Torus");
					torusEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Torus);
					m_SelectedEntity = torusEntity;
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
					entity.AddComponent<SpriteComponent>();
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Animated Sprite"))
				{
					Entity entity = SceneManager::CurrentScene()->CreateEntity("Animated Sprite");
					entity.AddComponent<AnimatedSpriteComponent>();
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Circle"))
				{
					Entity entity = SceneManager::CurrentScene()->CreateEntity("Circle");
					entity.AddComponent<CircleRendererComponent>();
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Tilemap"))
				{
					Entity entity = SceneManager::CurrentScene()->CreateEntity("Tilemap");
					entity.AddComponent<TilemapComponent>();
					m_SelectedEntity = entity;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("Audio Source", "", nullptr, false))
				{
					//TODO: create an audio source component
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Point Light"))
				{
					Entity entity = SceneManager::CurrentScene()->CreateEntity("Point Light");
					entity.AddComponent<PointLightComponent>();
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
				entity.AddComponent<CameraComponent>();
				m_SelectedEntity = entity;
			}
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
	dispatcher.Dispatch<AppOpenDocumentChange>([=](AppOpenDocumentChange&)
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

			if (parent)
				SceneGraph::Reparent(*childEntity, parent, SceneManager::CurrentScene()->GetRegistry());
			else
				SceneGraph::Unparent(*childEntity, SceneManager::CurrentScene()->GetRegistry());
			SceneManager::CurrentScene()->MakeDirty();
		}
		ImGui::EndDragDropTarget();
	}
}

void HierarchyPanel::Copy()
{
	CLIENT_DEBUG("Copied");
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pEntityElement = doc.NewElement("Entity");
	doc.InsertFirstChild(pEntityElement);
	SceneSerializer::SerializeEntity(pEntityElement, m_SelectedEntity);
	tinyxml2::XMLPrinter printer;
	doc.Accept(&printer);
	ImGui::SetClipboardText(printer.CStr());
}

void HierarchyPanel::Cut()
{
	Copy();
	Delete();
}

void HierarchyPanel::Paste()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.Parse(ImGui::GetClipboardText());

	if (error == tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pEntityElement = doc.FirstChildElement("Entity");
		if (pEntityElement)
		{
			SceneSerializer::DeserializeEntity(SceneManager::CurrentScene(), pEntityElement, true);

			SceneManager::CurrentScene()->MakeDirty();
		}
	}
}

void HierarchyPanel::Duplicate()
{
	CLIENT_DEBUG("Duplicated");
	SceneManager::CurrentScene()->DuplicateEntity(m_SelectedEntity, Entity());
}

void HierarchyPanel::Delete()
{
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