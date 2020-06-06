#include "ImGuiViewport.h"

void Viewport::Render(const char* title, const ImVec2& size)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags = ImGuiTabBarFlags_NoTooltip;
    ImGui::Begin("Viewport", NULL, flags);
    {
        if (m_WindowSize.x != size.x || m_WindowSize.y != size.y)
        {
            m_WindowSize = size;
            m_FrameBuffer->SetTextureSize(m_WindowSize.x, m_WindowSize.y);
        }

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 mouse_pos = ImGui::GetMousePos();

        //m_RelativeMousePosition = { mouse_pos.x - ImGui::GetWindowPos().x - 1.0f, mouse_pos.y - ImGui::GetWindowPos().y - 8.0f - ImGui::GetFontSize() };
        auto tex = m_FrameBuffer->GetTextureID();

        ImGui::GetWindowDrawList()->AddImage(
            (void*)tex,
            ImVec2(ImGui::GetItemRectMin().x - 20, ImGui::GetItemRectMin().y),
            ImVec2(pos.x + size.x, pos.y + size.y),
            ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}
