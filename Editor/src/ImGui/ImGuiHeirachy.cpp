#include "ImGuiHeirachy.h"

#include "IconsFontAwesome5.h"

ImGuiHeirachy::ImGuiHeirachy(bool* show)
	:m_Show(show), Layer("Heirachy")
{
	m_Scene = nullptr;
}

void ImGuiHeirachy::OnAttach()
{
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
		if (m_Scene != nullptr)
		{
			if (ImGui::TreeNode(m_Scene->GetSceneName().c_str()))
			{
				ImGui::Button("test");
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
