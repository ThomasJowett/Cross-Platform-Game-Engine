#include "ImGuiHeirachy.h"

#include "IconsFontAwesome5.h"
#include "ImGuiDockSpace.h"

#include "Scene/SceneManager.h"

ImGuiHeirachy::ImGuiHeirachy(bool* show)
	:m_Show(show), Layer("Heirachy")
{
	m_Scene = nullptr;
}

void ImGuiHeirachy::OnAttach()
{
	m_Scene = SceneManager::GetCurrentScene();
}

void ImGuiHeirachy::OnFixedUpdate()
{
}

void ImGuiHeirachy::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_SITEMAP" Heirachy", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			ImGuiDockSpace::SetFocussedWindow(this);
		}

		if (m_Scene != nullptr)
		{
			if (ImGui::TreeNodeEx(m_Scene->GetSceneName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_Scene->GetRegistry().each([&](auto entityID)
					{
						auto& name = m_Scene->GetRegistry().get<TagComponent>(entityID);
						Entity entity{ entityID, m_Scene.get(),  name };
						DrawNode(entity);
					});

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void ImGuiHeirachy::SetContext(const Ref<Scene>& scene)
{
	m_Scene = scene;
}

void ImGuiHeirachy::DrawNode(Entity entity)
{
	auto& tag = entity.GetComponent<TagComponent>().Tag;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntity = entity;
	}

	if (opened)
	{
		ImGui::TreePop();
	}
}
