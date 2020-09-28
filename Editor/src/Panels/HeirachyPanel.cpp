#include "HeirachyPanel.h"

#include "IconsFontAwesome5.h"
#include "MainDockSpace.h"

#include "Scene/SceneManager.h"

HeirachyPanel::HeirachyPanel(bool* show)
	:m_Show(show), Layer("Heirachy")
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

	if (ImGui::Begin(ICON_FA_SITEMAP" Heirachy", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
		{
			m_SelectedEntity = {};
		}

		if (SceneManager::s_CurrentScene != nullptr)
		{
			if (ImGui::TreeNodeEx(SceneManager::s_CurrentScene->GetSceneName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
