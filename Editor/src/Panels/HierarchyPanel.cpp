#include "HierarchyPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "Engine.h"

#include "Utilities/GeometryGenerator.h"
#include "Scene/SceneSerializer.h"
#include "Scene/SceneGraph.h"

HierarchyPanel::HierarchyPanel(bool* show)
	:m_Show(show), Layer("Hierarchy")
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::OnAttach()
{
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
		if (ImGui::BeginPopupContextWindow(0, 1, false))
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

					Mesh mesh(GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f), "Cube");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					cubeEntity.AddComponent<StaticMeshComponent>(mesh, material);
					cubeEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cube);
					m_SelectedEntity = cubeEntity;
				}
				if (ImGui::MenuItem("Sphere"))
				{
					Entity sphereEntity = SceneManager::CurrentScene()->CreateEntity("Sphere");

					Mesh mesh(GeometryGenerator::CreateSphere(0.5f, 16, 32), "Sphere");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					sphereEntity.AddComponent<StaticMeshComponent>(mesh, material);
					sphereEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Sphere);
					m_SelectedEntity = sphereEntity;
				}
				if (ImGui::MenuItem("Plane"))
				{
					Entity planeEntity = SceneManager::CurrentScene()->CreateEntity("Plane");

					Mesh mesh(GeometryGenerator::CreateGrid(1.0f, 1.0f, 2, 2, 1, 1), "Plane");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png")), 0);
					planeEntity.AddComponent<StaticMeshComponent>(mesh, material);
					planeEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Plane);
					m_SelectedEntity = planeEntity;
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					Entity cylinderEntity = SceneManager::CurrentScene()->CreateEntity("Cylinder");

					Mesh mesh(GeometryGenerator::CreateCylinder(0.5f, 0.5f, 1.0f, 32, 5), "Cylinder");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png")), 0);
					cylinderEntity.AddComponent<StaticMeshComponent>(mesh, material);
					cylinderEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cylinder);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Cone"))
				{
					Entity cylinderEntity = SceneManager::CurrentScene()->CreateEntity("Cone");

					Mesh mesh(GeometryGenerator::CreateCylinder(0.5f, 0.00001f, 1.0f, 32, 5), "Cone");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png")), 0);
					cylinderEntity.AddComponent<StaticMeshComponent>(mesh, material);
					cylinderEntity.AddComponent<PrimitiveComponent>(PrimitiveComponent::Shape::Cone);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Torus"))
				{
					Entity torusEntity = SceneManager::CurrentScene()->CreateEntity("Torus");

					Mesh mesh(GeometryGenerator::CreateTorus(1.0f, 0.4f, 32), "Torus");
					Material material(Shader::Create("Standard"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png")), 0);
					torusEntity.AddComponent<StaticMeshComponent>(mesh, material);
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
				if (ImGui::MenuItem("Point Light", "", nullptr, false))
				{
					//TODO: Create point light
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

		if (SceneManager::IsSceneLoaded())
		{
			if (ImGui::TreeNodeEx(SceneManager::CurrentScene()->GetSceneName().c_str(), ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_SpanAvailWidth
				| ImGuiTreeNodeFlags_Bullet
				| ImGuiTreeNodeFlags_OpenOnDoubleClick))
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_None))
					{
						Entity* childEntity = (Entity*)payload->Data;
						ASSERT(payload->DataSize == sizeof(Entity), "Drag-drop entity data not the correct size");

						SceneGraph::Unparent(*childEntity, SceneManager::CurrentScene()->GetRegistry());
						SceneManager::CurrentScene()->MakeDirty();
					}
					ImGui::EndDragDropTarget();
				}

				SceneManager::CurrentScene()->GetRegistry().each([&](auto entityID)
					{
						if (SceneManager::CurrentScene()->GetRegistry().valid(entityID))
						{
							Entity entity{ entityID, SceneManager::CurrentScene() };

							// Only draw a node for root entites, children are drawn recursively
							HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();
							if (!hierarchyComp || hierarchyComp->parent == entt::null)
								DrawNode(entity);
						}
					});

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void HierarchyPanel::SetSelectedEntity(Entity entity)
{
	m_SelectedEntity = entity;
}

void HierarchyPanel::DrawNode(Entity entity)
{
	std::string& name = entity.GetName();

	bool hasChildren = false;
	HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();

	if (hierarchyComp != nullptr && hierarchyComp->firstChild != entt::null)
		hasChildren = true;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
		| ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| (!hasChildren ? ImGuiTreeNodeFlags_Leaf : 0);

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
		ImGui::Text(name.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_None))
		{
			Entity* childEntity = (Entity*)payload->Data;
			ASSERT(payload->DataSize == sizeof(Entity), "Drag-drop entity data not the correct size");

			if (*childEntity != entity)
			{
				SceneGraph::Reparent(*childEntity, entity, SceneManager::CurrentScene()->GetRegistry());
				SceneManager::CurrentScene()->MakeDirty();
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right) || ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_SelectedEntity = entity;
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem(std::string(name.c_str() + std::to_string((uint32_t)entity)).c_str()))
	{
		if (ImGui::MenuItem(ICON_FA_CUT" Cut", "Ctrl + X", nullptr, HasSelection()))
			Cut();
		if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, HasSelection()))
			Copy();
		if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, ImGui::GetClipboardText() != nullptr))
			Paste();
		if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, HasSelection()))
			Duplicate();
		if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete", "Del", nullptr, HasSelection()))
		{
			entityDeleted = true;
		}
		ImGui::EndPopup();
	}


	if (opened)
	{
		if (hasChildren)
		{
			entt::entity child = entity.TryGetComponent<HierarchyComponent>()->firstChild;
			while (child != entt::null)
			{
				Entity childEntity = { child, SceneManager::CurrentScene() };

				DrawNode(childEntity);

				if (childEntity.IsValid())
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

void HierarchyPanel::Copy()
{
	CLIENT_DEBUG("Copied");
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pEntityElement = doc.NewElement("Entity");
	doc.InsertFirstChild(pEntityElement);
	SceneSerializer::SerializeEntity(pEntityElement, m_SelectedEntity);
	pEntityElement->SetAttribute("ID", Uuid());
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
			SceneSerializer::DeserializeEntity(SceneManager::CurrentScene(), pEntityElement);
			SceneManager::CurrentScene()->MakeDirty();
		}
	}
}

void HierarchyPanel::Duplicate()
{
	CLIENT_DEBUG("Duplicated");
	SceneManager::CurrentScene()->DuplicateEntity(m_SelectedEntity);
}

void HierarchyPanel::Delete()
{
	SceneManager::CurrentScene()->RemoveEntity(m_SelectedEntity);
	m_SelectedEntity = {};
}

bool HierarchyPanel::HasSelection() const
{
	return m_SelectedEntity;
}

void HierarchyPanel::SelectAll()
{
	CLIENT_ERROR("Multi select entites not implemented yet!");
}

bool HierarchyPanel::IsReadOnly() const
{
	return false;
}
