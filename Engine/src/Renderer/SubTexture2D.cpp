#include "stdafx.h"
#include "SubTexture2D.h"

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const Vector2f& min, const Vector2f& max)
	:m_Texture(texture)
{
	m_TexCoords[0] = { min.x, min.y };
	m_TexCoords[1] = { max.x, min.y };
	m_TexCoords[2] = { max.x, max.y };
	m_TexCoords[3] = { min.x, max.y };
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D>& texture, const Vector2f& cellCoords, const Vector2f& spriteSize, const Vector2f cellDims)
{
	Vector2f minCoord((cellCoords.x * spriteSize.x) / texture->GetWidth(), (cellCoords.y * spriteSize.y ) / texture->GetHeight());
	Vector2f maxCoord(((cellCoords.x + cellDims.x) * spriteSize.x) / texture->GetWidth(), ((cellCoords.y + cellDims.y) * spriteSize.y) / texture->GetHeight());
	return CreateRef<SubTexture2D>(texture, minCoord, maxCoord);
}