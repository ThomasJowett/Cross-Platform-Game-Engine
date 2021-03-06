#include "TextureView.h"

#include "IconsFontAwesome5.h"

TextureView::TextureView(bool* show, const std::filesystem::path& filepath)
	:Layer("TextureView"),m_Show(show), m_FilePath(filepath)
{

}
void TextureView::OnAttach()
{
	m_Texture = Texture2D::Create(m_FilePath.string());

	m_WindowName = ICON_FA_IMAGE + std::string(" ") + m_FilePath.filename().string() + "##" + std::to_string(m_Texture->GetRendererID());
}
void TextureView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, ImGuiWindowFlags_AlwaysHorizontalScrollbar))
	{
		ImGui::Image(m_Texture, ImVec2((float)m_Texture->GetWidth(), (float)m_Texture->GetHeight()));
	}
	ImGui::End();
}