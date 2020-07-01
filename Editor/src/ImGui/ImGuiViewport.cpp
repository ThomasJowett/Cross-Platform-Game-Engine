#include "ImGuiViewport.h"

ImGuiViewportPanel::ImGuiViewportPanel(bool* show, Ref<FrameBuffer> framebuffer)
	:m_Show(show), Layer("Viewport")
{
	m_FrameBuffer = framebuffer;
}

void ImGuiViewportPanel::OnUpdate(float deltaTime)
{
	FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
	if (m_ViewportSize.x != spec.Width && m_ViewportSize.y != spec.Height && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
	{
		m_FrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
	}
}

void ImGuiViewportPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Viewport", m_Show));
	{
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize.x != panelSize.x || m_ViewportSize.y != panelSize.y)
		{
			m_ViewportSize = panelSize;
		}

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 mouse_pos = ImGui::GetMousePos();

		m_RelativeMousePosition = { mouse_pos.x - ImGui::GetWindowPos().x - 1.0f, mouse_pos.y - ImGui::GetWindowPos().y - 8.0f - ImGui::GetFontSize() };
		auto tex = m_FrameBuffer->GetColourAttachment();

		ImGui::GetWindowDrawList()->AddImage(
			(void*)tex,
			ImVec2(ImGui::GetItemRectMin().x - 20, ImGui::GetItemRectMin().y),
			ImVec2(pos.x + panelSize.x, pos.y + panelSize.y),
			ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
