#include "HeirachyPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "Scene/SceneManager.h"

#include "Utilities/GeometryGenerator.h"

HeirachyPanel::HeirachyPanel(bool* show)
	:m_Show(show), Layer("Hierarchy")
{
}

void HeirachyPanel::OnAttach()
{
}

void HeirachyPanel::OnFixedUpdate()
{
}

void HeirachyPanel::OnImGuiRender()
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

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() || !m_SelectedEntity.BelongsToScene(SceneManager::s_CurrentScene.get()))
		{
			m_SelectedEntity = {};
		}

		// right click on a blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_SelectedEntity = SceneManager::s_CurrentScene->CreateEntity("New Entity");//TODO: make all the entities have a unique name
			}
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					Entity cubeEntity = SceneManager::s_CurrentScene->CreateEntity("Cube");

					Mesh mesh(GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f), "Cube");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					cubeEntity.AddComponent<StaticMeshComponent>(mesh, material);
					m_SelectedEntity = cubeEntity;
				}
				if (ImGui::MenuItem("Sphere"))
				{
					Entity sphereEntity = SceneManager::s_CurrentScene->CreateEntity("Sphere");

					Mesh mesh(GeometryGenerator::CreateSphere(0.5f, 16, 32), "Sphere");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					sphereEntity.AddComponent<StaticMeshComponent>(mesh, material);
					m_SelectedEntity = sphereEntity;
				}
				if (ImGui::MenuItem("Plane"))
				{
					Entity planeEntity = SceneManager::s_CurrentScene->CreateEntity("Plane");

					Mesh mesh(GeometryGenerator::CreateGrid(1.0f, 1.0f, 2, 2, 1, 1), "Plane");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					planeEntity.AddComponent<StaticMeshComponent>(mesh, material);
					m_SelectedEntity = planeEntity;
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					Entity cylinderEntity = SceneManager::s_CurrentScene->CreateEntity("Cylinder");

					Mesh mesh(GeometryGenerator::CreateCylinder(0.5f, 0.5f, 1.0f, 32, 5), "Cylinder");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					cylinderEntity.AddComponent<StaticMeshComponent>(mesh, material);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Cone"))
				{
					Entity cylinderEntity = SceneManager::s_CurrentScene->CreateEntity("Cone");

					Mesh mesh(GeometryGenerator::CreateCylinder(0.5f, 0.00001f, 1.0f, 32, 5), "Cone");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					cylinderEntity.AddComponent<StaticMeshComponent>(mesh, material);
					m_SelectedEntity = cylinderEntity;
				}
				if (ImGui::MenuItem("Torus"))
				{
					Entity torusEntity = SceneManager::s_CurrentScene->CreateEntity("Torus");

					Mesh mesh(GeometryGenerator::CreateTorus(1.0f, 0.4f, 32), "Torus");
					Material material(Shader::Create("NormalMap"));
					material.SetTint(Colours::RANDOM);
					material.AddTexture(Texture2D::Create(std::filesystem::path(Application::GetWorkingDirectory() / "resources" / "UVChecker.png").string()), 0);
					torusEntity.AddComponent<StaticMeshComponent>(mesh, material);
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
					Entity entity = SceneManager::s_CurrentScene->CreateEntity("Sprite");
					entity.AddComponent<SpriteComponent>();
					m_SelectedEntity = entity;
				}
				if (ImGui::MenuItem("Tilemap", "", nullptr, false))
				{
					//TODO: create tilemap
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
				Entity entity = SceneManager::s_CurrentScene->CreateEntity("Camera");
				entity.AddComponent<CameraComponent>();
				m_SelectedEntity = entity;
			}
			ImGui::EndPopup();
		}

		if (SceneManager::s_CurrentScene != nullptr)
		{
			if (ImGui::TreeNodeEx(SceneManager::s_CurrentScene->GetSceneName().c_str(), ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_SpanAvailWidth
				| ImGuiTreeNodeFlags_Bullet
				| ImGuiTreeNodeFlags_OpenOnDoubleClick))
			{
				SceneManager::s_CurrentScene->GetRegistry().each([&](auto entityID)
					{
						auto& name = SceneManager::s_CurrentScene->GetRegistry().get<TagComponent>(entityID);
						Entity entity{ entityID, SceneManager::s_CurrentScene.get(),  name };
						DrawNode(entity);
					});

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void HeirachyPanel::DrawNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
		| ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntity = entity;
	}

	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem(std::string(tag.c_str() + std::to_string((uint32_t)entity)).c_str()))
	{
		if (ImGui::MenuItem("Delete Entity"))
		{
			entityDeleted = true;
		}
		ImGui::EndPopup();
	}

	if (opened)
	{
		ImGui::TreePop();
	}

	if (entityDeleted)
	{
		if (m_SelectedEntity == entity)
			m_SelectedEntity = {};
		SceneManager::s_CurrentScene->RemoveEntity(entity);
	}
}
