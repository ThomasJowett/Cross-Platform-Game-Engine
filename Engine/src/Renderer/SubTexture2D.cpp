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
		m_CellsWide = (uint32_t)std::ceil(m_Texture->GetWidth() / m_SpriteWidth);
		m_CellsTall = (uint32_t)std::ceil(m_Texture->GetHeight() / m_SpriteHeight);

		m_PaddingRight = m_Texture->GetWidth() - (m_SpriteWidth * m_CellsWide);
		m_PaddingBottom = m_Texture->GetHeight() - (m_SpriteHeight * m_CellsTall);
	}

	SetCurrentCell(currentCell);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::SetCurrentCell(const uint32_t cell)
{
	if (cell != m_CurrentCell && cell < GetNumberOfCells())
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

void SubTexture2D::SetCellDimensions(uint32_t cellsWide, uint32_t cellsTall)
{
	m_CellsWide = std::max(cellsWide, (uint32_t)1);
	m_CellsTall = std::max(cellsTall, (uint32_t)1);

	m_SpriteWidth = (uint32_t)std::ceil(m_Texture->GetWidth() / m_CellsWide);
	m_SpriteHeight = (uint32_t)std::ceil(m_Texture->GetHeight() / m_CellsTall);

	m_PaddingBottom = 0;
	m_PaddingRight = 0;

	CalculateTextureCoordinates();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SubTexture2D::RecalculateCellsDimensions()
{
	if (m_Texture)
	{
		m_CellsWide = (uint32_t)std::ceil(m_Texture->GetWidth() / m_SpriteWidth);
		m_CellsTall = (uint32_t)std::ceil(m_Texture->GetHeight() / m_SpriteHeight);

		m_PaddingRight = m_Texture->GetWidth() - (m_SpriteWidth * m_CellsWide);
		m_PaddingBottom = m_Texture->GetHeight() - (m_SpriteHeight * m_CellsTall);
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
		ASSERT(cellCoordX < m_CellsWide, "Coords Cannot be wider than cells wide");
		ASSERT(cellCoordY < m_CellsTall, "Coords Cannot be higher than cells tall");
		float minX = (float)(cellCoordX * m_SpriteWidth) / (float)m_Texture->GetWidth() + m_Margin.x;
		float minY = (float)(((cellCoordY * m_SpriteHeight) + m_PaddingBottom) / (float)m_Texture->GetHeight() + m_Margin.y);
		float maxX = (float)((cellCoordX + 1) * m_SpriteWidth) / (float)m_Texture->GetWidth() - m_Margin.x;
		float maxY = (float)(((cellCoordY + 1) * m_SpriteHeight) + m_PaddingBottom) / (float)m_Texture->GetHeight() - m_Margin.y;
		m_TexCoords[0] = { minX, minY };
		m_TexCoords[1] = { maxX, minY };
		m_TexCoords[2] = { maxX, maxY };
		m_TexCoords[3] = { minX, maxY };
	}
}