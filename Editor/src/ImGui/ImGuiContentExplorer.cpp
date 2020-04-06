#include "ImGuiContentExplorer.h"

#include "ImGui/imgui.h"

ImGuiContentExplorer::ImGuiContentExplorer(bool* show)
	:m_Show(show), Layer("ContentExplorer")
{
}

void ImGuiContentExplorer::OnImGuiRender()
{
	if (*m_Show)
	{
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		if(ImGui::Begin("Content Explorer", m_Show))
		{

		}
		ImGui::End();
	}
}
