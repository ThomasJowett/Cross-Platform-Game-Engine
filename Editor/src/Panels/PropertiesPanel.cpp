#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"

#include "Scene/SceneManager.h"

#include <cstring>

PropertiesPanel::PropertiesPanel(bool* show, HeirachyPanel* heirachyPanel)
	:Layer("Properties"), m_Show(show), m_HeirachyPanel(heirachyPanel)
{
}

void PropertiesPanel::OnAttach()
{
}

void PropertiesPanel::OnFixedUpdate()
{
}

void PropertiesPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_TOOLS" Properties", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		Entity entity = m_HeirachyPanel->GetSelectedEntity();
		if (entity)
		{
			ImGui::BeginGroup();
			DrawComponents(entity);
			ImGui::Separator();
			DrawAddComponent(entity);
			ImVec2 available = ImGui::GetContentRegionAvail();
			ImGui::Dummy(available);
			ImGui::EndGroup();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
				{
					std::filesystem::path* file = (std::filesystem::path*)payload->Data;

					if (file->extension() == ".staticmesh" && !entity.HasComponent<StaticMeshComponent>())
					{
						//TODO: Store the material in the mesh file
						Mesh mesh(*file);

						Material material(Shader::Create("NormalMap"));

						material.AddTexture(Texture2D::Create(Application::GetWorkingDirectory().string() + "\\resources\\UVChecker.png"), 0);

						entity.AddComponent<StaticMeshComponent>(mesh, material);
					}

					CLIENT_DEBUG(file->string());
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
	ImGui::End();
}

void PropertiesPanel::DrawComponents(Entity entity)
{
	//Tag------------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::strncpy(buffer, tag.c_str(), sizeof(buffer));

		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
			SceneManager::s_CurrentScene->MakeDirty();
		}
	}

	//Transform------------------------------------------------------------------------------------------------------------
	DrawComponent<TransformComponent>("Transform", entity, [](auto& transform) 
		{
			ImGui::Transform(transform.Position, transform.Rotation, transform.Scale); 
		}, false);


	//Sprite--------------------------------------------------------------------------------------------------------------
	DrawComponent<SpriteComponent>(ICON_FA_IMAGE" Sprite", entity, [](auto& sprite)
		{
			float* tint[4] = { &sprite.Tint.r, &sprite.Tint.g, &sprite.Tint.b, &sprite.Tint.a };

			if (ImGui::ColorEdit4("Tint", tint[0]))
			{
				SceneManager::s_CurrentScene->MakeDirty();
			}
		});

	//Static Mesh------------------------------------------------------------------------------------------------------------
	DrawComponent<StaticMeshComponent>(ICON_FA_SHAPES" Static Mesh", entity, [](auto& staticMesh)
		{
			//TODO: create static mesh properties

			/* static mesh properties */
		});

	//Native Script------------------------------------------------------------------------------------------------------------
	DrawComponent<NativeScriptComponent>(ICON_FA_FILE_CODE" Native Script", entity, [](auto& script)
		{
			ImGui::Text("%s", script.Name.c_str());
		});

	//Camera------------------------------------------------------------------------------------------------------------
	DrawComponent<CameraComponent>(ICON_FA_VIDEO" Camera", entity, [](auto& cameraComp)
		{
			auto& camera = cameraComp.Camera;

			ImGui::Checkbox("Primary", &cameraComp.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &cameraComp.FixedAspectRatio);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };

			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjection((SceneCamera::ProjectionType)i);
						SceneManager::s_CurrentScene->MakeDirty();
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}


			switch (camera.GetProjectionType())
			{
			case SceneCamera::ProjectionType::perspective:
			{
				float fov = (float)RadToDeg(camera.GetVerticalFov());
				if (ImGui::DragFloat("Fov", &fov, 1.0f, 1.0f, 180.0f))
				{
					camera.SetVerticalFov((float)DegToRad(fov));
					SceneManager::s_CurrentScene->MakeDirty();
				}

				float nearClip = camera.GetPerspectiveNear();
				if (ImGui::DragFloat("Near Clip##Perspective", &nearClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveNear(nearClip);
					SceneManager::s_CurrentScene->MakeDirty();
				}

				float farClip = camera.GetPerspectiveFar();
				if (ImGui::DragFloat("Far Clip##Perspective", &farClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveFar(farClip);
					SceneManager::s_CurrentScene->MakeDirty();
				}

				camera.RecalculateProjection();
				break;
			}
			case SceneCamera::ProjectionType::orthographic:
			{
				float size = camera.GetOrthoSize();
				if (ImGui::DragFloat("Size", &size, 0.1f, 0.0f, 100.0f))
				{
					camera.SetOrthoSize(size);
					SceneManager::s_CurrentScene->MakeDirty();
				}

				float nearClip = camera.GetOrthoNear();
				if (ImGui::DragFloat("Near Clip##Ortho", &nearClip))
				{
					camera.SetOrthoNear(nearClip);
					SceneManager::s_CurrentScene->MakeDirty();
				}

				float farClip = camera.GetOrthoFar();
				if (ImGui::DragFloat("Far Clip##Ortho", &farClip))
				{
					camera.SetOrthoFar(farClip);
					SceneManager::s_CurrentScene->MakeDirty();
				}

				camera.RecalculateProjection();
				break;
			}
			}
		});
}

void PropertiesPanel::DrawAddComponent(Entity entity)
{
	ImGui::Dummy({ 0,0 });
	float width = ImGui::GetContentRegionAvailWidth();
	ImGui::SameLine((width / 2.0f) - (ImGui::CalcTextSize("Add Component").x / 2.0f));
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Components");
	}

	if (ImGui::BeginPopup("Components"))
	{
		AddComponentMenuItem<CameraComponent>("Camera", entity);
		AddComponentMenuItem<SpriteComponent>("Sprite", entity);
		AddComponentMenuItem<StaticMeshComponent>("Static Mesh", entity);

		if (ImGui::BeginMenu("Native Script", !entity.HasComponent<NativeScriptComponent>() && Factory<ScriptableEntity>::GetMap()->size() > 0))
		{
			for (auto&& [key, value] : *Factory<ScriptableEntity>::GetMap())
			{
				if (ImGui::MenuItem(key.c_str()))
				{
					entity.AddComponent<NativeScriptComponent>().Bind(key);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
