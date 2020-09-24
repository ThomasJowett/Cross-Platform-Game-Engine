#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "ImGui/ImGuiTransform.h"

#define DegToRad(degrees) (degrees * (PI/180))
#define RadToDeg(radians) (radians * (180/PI))

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
			DrawComponents(entity);
			ImGui::Separator();
		}
	}
	ImGui::End();
}

void PropertiesPanel::DrawComponents(Entity entity)
{
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

	if (entity.HasComponent<TransformComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;

			ImGui::Transform(transform);

			ImGui::TreePop();
		}
	}

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

	if (entity.HasComponent<StaticMeshComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(StaticMeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_SHAPES" Static Mesh"))
		{
			auto& mesh = entity.GetComponent<StaticMeshComponent>();

			ImGui::TreePop();
		}
	}

	if (entity.HasComponent<NativeScriptComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_FILE_CODE" Native Script"))
		{
			auto& script = entity.GetComponent<NativeScriptComponent>();

			ImGui::TreePop();
		}
	}

	if (entity.HasComponent<CameraComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(NativeScriptComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, ICON_FA_CAMERA" Camera"))
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
}
