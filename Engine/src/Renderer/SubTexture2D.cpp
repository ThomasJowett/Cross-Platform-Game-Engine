#include "stdafx.h"
#include "SubTexture2D.h"

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, uint32_t spriteWidth, uint32_t spriteHeight, uint32_t currentCell)
	:m_Texture(texture), m_SpriteWidth(spriteWidth), m_SpriteHeight(spriteHeight)
{
	m_CellsWide = texture->GetWidth() / spriteWidth;
	m_CellsTall = texture->GetHeight() / spriteHeight;

	SetCurrentCell(currentCell);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::SetCurrentCell(const uint32_t cell)
{
	if (cell != m_CurrentCell)
	{
		m_CurrentCell = cell;
		CalculateTextureCoordinates();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::CalculateTextureCoordinates()
{
	uint32_t cellCoordX = (m_CurrentCell / m_CellsWide);
	uint32_t cellCoordY = (m_CurrentCell / m_CellsTall);
	Vector2f min((float)(cellCoordX * m_SpriteWidth) /(float)m_Texture->GetWidth(), (float)((cellCoordY * m_SpriteHeight) / (float)m_Texture->GetHeight()));
	Vector2f max((float)((cellCoordX + 1) * m_SpriteWidth) / (float)m_Texture->GetWidth(), (float)((cellCoordY + 1) * m_SpriteHeight) / (float)m_Texture->GetHeight());
	m_TexCoords[0] = { min.x, min.y };
	m_TexCoords[1] = { max.x, min.y };
	m_TexCoords[2] = { max.x, max.y };
	m_TexCoords[3] = { min.x, max.y };
}