#include "stdafx.h"
#include "Material.h"

Material::Material(Ref<Shader> shader)
	:m_Shader(shader)
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
