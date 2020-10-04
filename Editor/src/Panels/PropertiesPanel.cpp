#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"

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
		strcpy_s(buffer, sizeof(buffer), tag.c_str());

		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	//Transform------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<TransformComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;

			ImGui::Transform(transform);

			ImGui::TreePop();
		}
	}

	//Sprite--------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<SpriteComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(SpriteComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_IMAGE" Sprite"))
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();

			float* tint[4] = { &sprite.Tint.r, &sprite.Tint.g, &sprite.Tint.b, &sprite.Tint.a };

			ImGui::ColorEdit4("Tint", tint[0]);

			ImGui::TreePop();
		}
	}

	//Static Mesh------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<StaticMeshComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(StaticMeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_SHAPES" Static Mesh"))
		{
			auto& mesh = entity.GetComponent<StaticMeshComponent>();

			ImGui::TreePop();
		}
	}

	//Native Script------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<NativeScriptComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_FILE_CODE" Native Script"))
		{
			auto& script = entity.GetComponent<NativeScriptComponent>();

			ImGui::Text(script.Name.c_str());

			ImGui::TreePop();
		}
	}

	//Camera------------------------------------------------------------------------------------------------------------
	if (entity.HasComponent<CameraComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_VIDEO" Camera"))
		{
			auto& cameraComp = entity.GetComponent<CameraComponent>();
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
				float fov = RadToDeg(camera.GetVerticalFov());
				if (ImGui::DragFloat("Fov", &fov, 1.0f, 1.0f, 180.0f))
				{
					camera.SetVerticalFov(DegToRad(fov));
				}

				float nearClip = camera.GetPerspectiveNear();
				if (ImGui::DragFloat("Near Clip##Perspective", &nearClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveNear(nearClip);
				}

				float farClip = camera.GetPerspectiveFar();
				if (ImGui::DragFloat("Far Clip##Perspective", &farClip, 1.0f, 0.001f, 10000.0f))
				{
					camera.SetPerspectiveFar(farClip);
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
				}

				float nearClip = camera.GetOrthoNear();
				if (ImGui::DragFloat("Near Clip##Ortho", &nearClip))
				{
					camera.SetOrthoNear(nearClip);
				}

				float farClip = camera.GetOrthoFar();
				if (ImGui::DragFloat("Far Clip##Ortho", &farClip))
				{
					camera.SetOrthoFar(farClip);
				}

				camera.RecalculateProjection();
				break;
			}
			}

			ImGui::Text(camera.GetProjectionMatrix().to_string().c_str());

			ImGui::TreePop();
		}
	}

	//Add Component Button ---------------------------------------------------------------------------------------------
	
}

void PropertiesPanel::DrawAddComponent(Entity entity)
{
	ImGui::Dummy({0,0 });
	float width = ImGui::GetContentRegionAvailWidth();
	ImGui::SameLine((width / 2.0f) - (ImGui::CalcTextSize("Add Component").x/2.0f));
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("Components");
	}

	if (ImGui::BeginPopup("Components"))
	{

		if (ImGui::MenuItem("Camera", nullptr, nullptr, !entity.HasComponent<CameraComponent>()))
		{
			entity.AddComponent<CameraComponent>();
		}
		if (ImGui::MenuItem("Sprite", nullptr, nullptr, !entity.HasComponent<SpriteComponent>()))
		{
			entity.AddComponent<SpriteComponent>();
		}
		//if (ImGui::MenuItem("Static Mesh", nullptr, nullptr, !entity.HasComponent<StaticMeshComponent>()))
		//{
		//	entity.AddComponent<StaticMeshComponent>();
		//}
		if (ImGui::BeginMenu("Native Script", !entity.HasComponent<NativeScriptComponent>()))
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
