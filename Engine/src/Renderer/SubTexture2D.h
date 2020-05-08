#pragma once

#include "Texture.h"

class SubTexture2D
{
public:
	SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& min, const Vector2f& max);

	const Ref<Texture2D> GetTexture() const { return m_Texture; }
	const Vector2f* GetTextureCoordinates() const { return m_TexCoords; }

	static Ref<SubTexture2D> Create(const Ref<Texture2D>& texture, const Vector2f& cellCoords, const Vector2f& spriteSize, const Vector2f cellDims = { 1,1 });
private:
	Ref<Texture2D> m_Texture;
	Vector2f m_TexCoords[4];
};