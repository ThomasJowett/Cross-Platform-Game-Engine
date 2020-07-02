#include "ImGuiViewport.h"

ImGuiViewportPanel::ImGuiViewportPanel(bool* show, Ref<FrameBuffer> framebuffer)
	:m_Show(show), Layer("Viewport")
{
	m_FrameBuffer = framebuffer;
}

void ImGuiViewportPanel::OnFixedUpdate()
{
	FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
	if (((uint32_t)m_ViewportSize.x != spec.Width || (uint32_t)m_ViewportSize.y != spec.Height) && (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f))
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

	ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);

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

		ImGui::Image((void*)tex, m_ViewportSize, ImVec2(1,0), ImVec2(0,1));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset", ImGuiDragDropFlags_None))
			{
				std::filesystem::path* file = (std::filesystem::path*)payload->Data;
				CLIENT_DEBUG(file->string());
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
