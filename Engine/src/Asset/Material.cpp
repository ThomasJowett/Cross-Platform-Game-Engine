#include "stdafx.h"
#include "Material.h"

#include "TinyXml2/tinyxml2.h"
#include "Utilities/SerializationUtils.h"
#include "Utilities/FileUtils.h"

Ref<Material> Material::s_DefaultMaterial = nullptr;

Material::Material()
{
}

Material::Material(const std::filesystem::path& filepath)
{
	Load(filepath);
}

Material::Material(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
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
	PROFILE_FUNCTION();
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
	{
		return false;
	}

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(absolutePath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load material file: {0}", absolutePath);
			return false;
		}
	}
	else
	{
		ENGINE_ERROR("Could not load material {0}. {1} on line {2}", absolutePath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();

	return true;
}

bool Material::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load material from memory: {0}", filepath);
			return false;
		}
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial(const std::filesystem::path& filepath) const
{
	PROFILE_FUNCTION();
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

	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);

	tinyxml2::XMLError error = doc.SaveFile(absolutePath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Material::SaveMaterial() const
{
	return SaveMaterial(m_Filepath);
}

Ref<Material> Material::GetDefaultMaterial()
{
	PROFILE_FUNCTION();
	if (!s_DefaultMaterial)
	{
		s_DefaultMaterial = CreateRef<Material>();

		s_DefaultMaterial->m_Filepath = "DefaultMaterial";

		const uint32_t textureSize = 8;
		uint32_t textureData[textureSize][textureSize];

		for (uint32_t i = 0; i < textureSize; i++)
		{
			for (uint32_t j = 0; j < textureSize; j++)
			{
				textureData[i][j] = ((i + j) % 2) ? 0xff404040 : 0xff969696;
			}
		}

		Ref<Texture2D> texture = Texture2D::Create(textureSize, textureSize, Texture::Format::RGBA, &textureData);

		s_DefaultMaterial->AddTexture(texture, 0);
	}
	return s_DefaultMaterial;
}

bool Material::LoadXML(tinyxml2::XMLDocument* doc)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLElement* pRoot = doc->FirstChildElement("Material");

	if (!pRoot)
	{
		ENGINE_ERROR("Could not read material file, no material node");
		return false;
	}

	m_Textures.clear();

	SerializationUtils::Decode(pRoot->FirstChildElement("Tint"), m_Tint);

	m_TwoSided = pRoot->BoolAttribute("IsTwoSided", false);
	m_Transparent = pRoot->BoolAttribute("Transparent", false);
	m_CastShadows = pRoot->BoolAttribute("CastShadows", true);

	const char* shaderChar = pRoot->Attribute("Shader");

	if (shaderChar)
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
	return true;
}
