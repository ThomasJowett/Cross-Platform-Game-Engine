#include "ImGuiStaticMeshView.h"

ImGuiStaticMeshView::ImGuiStaticMeshView(bool* show)
	:Layer("StaticMeshView"),m_Show(show)
{
}

void ImGuiStaticMeshView::OnAttach()
{
}

void ImGuiStaticMeshView::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Insert Mesh Name Here"), m_Show)
	{

	}
	ImGui::End();
}
