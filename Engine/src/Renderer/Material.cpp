#include "stdafx.h"
#include "Material.h"

Material::Material(Ref<Shader> shader, Colour tint)
	:m_Shader(shader), m_Tint(tint)
{
}

void Material::BindTextures() const
{
	for (auto&&[texture, slot] : m_Textures)
	{
		texture->Bind(slot);
	}
}

void Material::AddTexture(Ref<Texture> texture, uint32_t slot)
{
	m_Textures[texture] = slot;
}
