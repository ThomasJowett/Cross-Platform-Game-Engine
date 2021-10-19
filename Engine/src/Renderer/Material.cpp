#include "stdafx.h"
#include "Material.h"

#include "TinyXml2/tinyxml2.h"

Material::Material(Ref<Shader> shader, Colour tint)
	:m_Shader(shader), m_Tint(tint)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::BindTextures() const
{
	for (auto&&[texture, slot] : m_Textures)
	{
		texture->Bind(slot);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::AddTexture(Ref<Texture> texture, uint32_t slot)
{
	m_Textures[texture] = slot;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::LoadMaterial(const std::filesystem::path& filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot;

	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::LoadMaterial()
{
	LoadMaterial(m_Filepath);
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial(const std::filesystem::path& filepath) const
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Material");

	//TODO:save material attributes

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial() const
{
	return SaveMaterial(m_Filepath);
}
