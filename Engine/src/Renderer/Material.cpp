#include "stdafx.h"
#include "Material.h"

#include "TinyXml2/tinyxml2.h"
#include "Utilities/SerializationUtils.h"
#include "Utilities/FileUtils.h"

Material::Material()
{
	DefaultMaterial();
}

Material::Material(const std::filesystem::path& filepath)
{
	Load(filepath);
}

Material::Material(const std::string& shader, Colour tint)
	:m_Shader(shader), m_Tint(tint), Asset()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::BindTextures() const
{
	for (auto&& [slot, texture] : m_Textures)
	{
		texture->Bind(slot);
	}
}

Ref<Texture2D> Material::GetTexture(uint32_t slot)
{
	if (m_Textures.find(slot) == m_Textures.end())
		return nullptr;
	return m_Textures.at(slot);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Material::AddTexture(Ref<Texture2D> texture, uint32_t slot)
{
	if (texture)
		m_Textures[slot] = texture;
	else
		m_Textures.erase(slot);
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::Load(const std::filesystem::path& filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		DefaultMaterial();
		return false;
	}

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Material");

		if (!pRoot)
		{
			ENGINE_ERROR("Could not read material file, no material node {0}", filepath);
			DefaultMaterial();
			return false;
		}

		m_Filepath = filepath;
		m_Textures.clear();

		SerializationUtils::Decode(pRoot->FirstChildElement("Tint"), m_Tint);

		m_TwoSided = pRoot->BoolAttribute("IsTwoSided", false);
		m_Transparent = pRoot->BoolAttribute("Transparent", false);
		m_CastShadows = pRoot->BoolAttribute("CastShadows", true);

		const char* shaderChar = pRoot->Attribute("Shader", "Standard");

		m_Shader = shaderChar;

		tinyxml2::XMLElement* pTextureElement = pRoot->FirstChildElement("Texture");

		while (pTextureElement)
		{
			Ref<Texture2D> texture;
			SerializationUtils::Decode(pTextureElement, texture);

			if (!texture)
			{
				texture = Texture2D::Create("");
			}

			uint32_t slot = 1;

			pTextureElement->QueryUnsignedAttribute("Slot", &slot);

			AddTexture(texture, slot);
			pTextureElement = pTextureElement->NextSiblingElement("Texture");
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial(const std::filesystem::path& filepath) const
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Material");

	doc.InsertFirstChild(pRoot);

	SerializationUtils::Encode(pRoot->InsertNewChildElement("Tint"), m_Tint);

	pRoot->SetAttribute("IsTwoSided", m_TwoSided);
	pRoot->SetAttribute("Transparent", m_Transparent);
	pRoot->SetAttribute("CastShadows", m_CastShadows);

	pRoot->SetAttribute("Shader", m_Shader.c_str());

	for (auto&& [slot, texture] : m_Textures)
	{
		tinyxml2::XMLElement* pTextureElement = pRoot->InsertNewChildElement("Texture");

		pTextureElement->SetAttribute("Slot", slot);

		SerializationUtils::Encode(pTextureElement, texture);
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial() const
{
	return SaveMaterial(m_Filepath);
}

void Material::DefaultMaterial()
{
	m_Filepath = "";
	m_Tint.SetColour(Colours::WHITE);
	m_TwoSided = false;
	m_Shader = "Standard";
	m_CastShadows = true;
	m_TilingFactor = 1.0f;
	const uint32_t textureSize = 8;
	uint32_t textureData[textureSize][textureSize];

	for (uint32_t i = 0; i < textureSize; i++)
	{
		for (uint32_t j = 0; j < textureSize; j++)
		{
			textureData[i][j] = ((i + j) % 2) ? 0xff404040 : 0xff969696;
		}
	}

	Ref<Texture2D> texture = Texture2D::Create(textureSize, textureSize);
	texture->SetData(&textureData, sizeof(uint32_t) * textureSize * textureSize);

	AddTexture(texture, 0);
}