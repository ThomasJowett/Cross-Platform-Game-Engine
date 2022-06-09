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
		m_CellsWide = (uint32_t)std::ceil(m_Texture->GetWidth() / spriteWidth);
		m_CellsTall = (uint32_t)std::ceil(m_Texture->GetHeight() / spriteHeight);
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
		m_CellsWide = (uint32_t)std::ceil(m_Texture->GetWidth() / m_SpriteWidth);
		m_CellsTall = (uint32_t)std::ceil(m_Texture->GetHeight() / m_SpriteHeight);
	}
	CalculateTextureCoordinates();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::CalculateTextureCoordinates()
{
	if (m_Texture && m_CellsWide > 0)
	{
		div_t div = std::div((int)m_CurrentCell, (int)m_CellsWide);
		uint32_t cellCoordX = div.rem;
		uint32_t cellCoordY = m_CellsTall - div.quot - 1;
		float minX = (float)(cellCoordX * m_SpriteWidth) / (float)m_Texture->GetWidth() + m_Margin.x;
		float minY = (float)(cellCoordY * m_SpriteHeight) / (float)m_Texture->GetHeight() + m_Margin.y;
		float maxX = (float)((cellCoordX + 1) * m_SpriteWidth) / (float)m_Texture->GetWidth() - m_Margin.x;
		float maxY = (float)((cellCoordY + 1) * m_SpriteHeight) / (float)m_Texture->GetHeight() - m_Margin.x;
		m_TexCoords[0] = { minX, minY };
		m_TexCoords[1] = { maxX, minY };
		m_TexCoords[2] = { maxX, maxY };
		m_TexCoords[3] = { minX, maxY };
	}
}