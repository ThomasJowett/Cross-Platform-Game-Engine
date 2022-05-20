#include "TilesetView.h"

#include "IconsFontAwesome5.h"

#include "Engine.h"

TilesetView::TilesetView(bool* show, std::filesystem::path filepath)
	:Layer("TilesetView"), m_Show(show), m_Filepath(filepath)
{
}

void TilesetView::OnAttach()
{
	m_WindowName = ICON_FA_TH + std::string(" " + m_Filepath.filename().string());

	m_Tileset = CreateRef<Tileset>(m_Filepath);
}

void TilesetView::OnImGuiRender()
{
	if (!*m_Show)
		return;

	ImVec2 displaySize = ImVec2((float)m_Tileset->GetSubTexture()->GetTexture()->GetWidth() * m_Zoom,
		(float)m_Tileset->GetSubTexture()->GetTexture()->GetHeight() * m_Zoom);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		ImGui::Image(m_Tileset->GetSubTexture()->GetTexture(), displaySize);
	}
	ImGui::End();
}

void TilesetView::Save()
{
}

void TilesetView::SaveAs()
{
}
