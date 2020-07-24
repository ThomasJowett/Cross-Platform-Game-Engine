#include "ImGuiProperties.h"

#include "IconsFontAwesome5.h"
#include "ImGuiDockSpace.h"

ImGuiProperties::ImGuiProperties(bool* show)
	:Layer("Properties"), m_Show(show)
{
}

void ImGuiProperties::OnAttach()
{
}

void ImGuiProperties::OnFixedUpdate()
{
}

void ImGuiProperties::OnImGuiRender()
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
			ImGuiDockSpace::SetFocussedWindow(this);
		}
	}
	ImGui::End();
}
