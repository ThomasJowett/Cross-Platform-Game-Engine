#include "PropertiesPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

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

			float* position[3] = { &transform.m[0][3], &transform.m[1][3], &transform.m[2][3] };

			ImGui::DragFloat3("Position", position[0], 0.1f);

			Vector3f scale = transform.ExtractScale();

			ImGui::DragFloat3("Scale", &scale[0]);

			Quaternion rotation = transform.ExtractRotation();

			ImGui::DragFloat4("Rotation", &rotation[0]);

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
}
