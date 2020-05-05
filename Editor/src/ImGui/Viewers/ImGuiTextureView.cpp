#include "ImGuiTextureView.h"

ImGuiTextureView::ImGuiTextureView(bool* show, std::filesystem::path filepath)
	:Layer("TextureView"),m_Show(show), m_FilePath(filepath)
{

}
void ImGuiTextureView::OnAttach()
{
	m_Texture = Texture2D::Create(m_FilePath.string());

	m_WindowName = m_FilePath.filename().string() + "##" + std::to_string(m_Texture->GetRendererID());
}
void ImGuiTextureView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		ImGui::Image(m_Texture, ImVec2((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight()));
	}
	ImGui::End();
}