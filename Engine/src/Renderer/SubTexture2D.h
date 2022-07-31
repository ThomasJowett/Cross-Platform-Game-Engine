#pragma once

#include "Texture.h"
#include "math/Vector2f.h"

class SubTexture2D
{
public:
	SubTexture2D();
	SubTexture2D(const Ref<Texture2D>& texture, uint32_t spriteWidth, uint32_t spriteHeight, uint32_t currentCell = 0);

	const Ref<Texture2D> GetTexture() const { return m_Texture; }
	Ref<Texture2D>& GetTexture() { return m_Texture; }
	const Vector2f* GetTextureCoordinates() const { return m_TexCoords; }

	void SetCurrentCell(const uint32_t cell);

	uint32_t GetSpriteWidth() const { return m_SpriteWidth; }
	uint32_t GetSpriteHeight() const { return m_SpriteHeight; }

	uint32_t GetCellsWide() const { return m_CellsWide; }
	uint32_t GetCellsTall() const { return m_CellsTall; }
	uint32_t GetNumberOfCells() const { return m_CellsWide * m_CellsTall; }

	void SetSpriteDimensions(uint32_t spriteWidth, uint32_t spriteHeight);
	void SetCellDimensions(uint32_t cellsWide, uint32_t cellsTall);

	void RecalculateCellsDimensions();

	Vector2f GetMargin() { return m_Margin; }
	void SetMargin(const Vector2f& margin) { m_Margin = margin; }
private:
	void CalculateTextureCoordinates();
	Ref<Texture2D> m_Texture;
	Vector2f m_TexCoords[4];
	Vector2f m_Margin = { 0.0001f, 0.0001f };

	uint32_t m_CurrentCell;

	uint32_t m_SpriteHeight, m_SpriteWidth;
	uint32_t m_CellsWide = 1, m_CellsTall = 1;
};