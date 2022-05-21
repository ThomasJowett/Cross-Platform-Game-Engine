#include "stdafx.h"
#include "SubTexture2D.h"

SubTexture2D::SubTexture2D()
	:m_SpriteWidth(32), m_SpriteHeight(32), m_CurrentCell(0)
{
}

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, uint32_t spriteWidth, uint32_t spriteHeight, uint32_t currentCell)
	: m_Texture(texture), m_SpriteWidth(spriteWidth), m_SpriteHeight(spriteHeight)
{
	if (m_Texture)
	{
		m_CellsWide = m_Texture->GetWidth() / spriteWidth;
		m_CellsTall = m_Texture->GetHeight() / spriteHeight;
	}

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

void SubTexture2D::SetSpriteDimensions(uint32_t spriteWidth, uint32_t spriteHeight)
{
	if (spriteWidth == 0)
		m_SpriteWidth = 1;
	else
		m_SpriteWidth = spriteWidth;
	if (spriteHeight == 0)
		m_SpriteHeight = 1;
	else
		m_SpriteHeight = spriteHeight;

	RecalculateCellsDimensions();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::RecalculateCellsDimensions()
{
	if (m_Texture)
	{
		m_CellsWide = m_Texture->GetWidth() / m_SpriteWidth;
		m_CellsTall = m_Texture->GetHeight() / m_SpriteHeight;
	}
	CalculateTextureCoordinates();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::CalculateTextureCoordinates()
{
	if (m_Texture)
	{
		div_t div = std::div((int)m_CurrentCell, (int)m_CellsWide);
		uint32_t cellCoordX = div.rem;
		uint32_t cellCoordY = m_CellsTall - div.quot - 1;
		Vector2f min((float)(cellCoordX * m_SpriteWidth) / (float)m_Texture->GetWidth(), (float)((cellCoordY * m_SpriteHeight) / (float)m_Texture->GetHeight()));
		Vector2f max((float)((cellCoordX + 1) * m_SpriteWidth) / (float)m_Texture->GetWidth(), (float)((cellCoordY + 1) * m_SpriteHeight) / (float)m_Texture->GetHeight());
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}
}