#pragma once

#include "Texture.h"

class SubTexture2D
{
public:
	SubTexture2D(const Ref<Texture2D>& texture, uint32_t spriteWidth, uint32_t spriteHeight, uint32_t currentCell = 0);

	const Ref<Texture2D> GetTexture() const { return m_Texture; }
	Ref<Texture2D> GetTexture() { return m_Texture; }
	const Vector2f* GetTextureCoordinates() const { return m_TexCoords; }

	void SetCurrentCell(const uint32_t cell);

	uint32_t GetSpriteWidth() { return m_SpriteWidth; }
	uint32_t GetSpriteHeight() { return m_SpriteHeight; }

private:
	void CalculateTextureCoordinates();
	Ref<Texture2D> m_Texture;
	Vector2f m_TexCoords[4];

	uint32_t m_CurrentCell;

	uint32_t m_SpriteHeight, m_SpriteWidth;
	uint32_t m_CellsWide, m_CellsTall;
};