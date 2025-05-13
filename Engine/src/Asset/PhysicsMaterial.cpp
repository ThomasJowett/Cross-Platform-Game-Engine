#include "stdafx.h"
#include "PhysicsMaterial.h"
#include "Core/Application.h"

#include "TinyXml2/tinyxml2.h"

Ref<PhysicsMaterial> PhysicsMaterial::s_DefaultPhysicsMaterial = nullptr;

Ref<PhysicsMaterial> PhysicsMaterial::GetDefaultPhysicsMaterial()
{
	if (!s_DefaultPhysicsMaterial)
		s_DefaultPhysicsMaterial = CreateRef<PhysicsMaterial>();
	return s_DefaultPhysicsMaterial;
}

bool PhysicsMaterial::LoadXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* pRoot = doc->FirstChildElement("PhysicsMaterial");

	if (!pRoot)
	{
		ENGINE_ERROR("Could not read phyics material file, no physics material node");
		return false;
	}

	m_Density = pRoot->FloatAttribute("Density", m_Density);
	m_Friction = pRoot->FloatAttribute("Friction", m_Friction);
	m_Restitution = pRoot->FloatAttribute("Restitution", m_Restitution);
	return true;
}

PhysicsMaterial::PhysicsMaterial(const std::filesystem::path& filepath)
{
	if (!Load(filepath))
		m_Filepath = "DefaultPhysicsMaterial";
}

PhysicsMaterial::PhysicsMaterial(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	if (!Load(filepath, data))
		m_Filepath = "DefaultPhysicsMaterial";
}

PhysicsMaterial::PhysicsMaterial(float density, float friction, float restitution)
	:m_Density(density), m_Friction(friction), m_Restitution(restitution)
{
}

bool PhysicsMaterial::Load(const std::filesystem::path& filepath)
{
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
		return false;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(absolutePath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		if(!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load physics material file: {0}", absolutePath);
			return false;
		}
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

bool PhysicsMaterial::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load physics material file: {0}", filepath);
			return false;
		}
	}
	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

bool PhysicsMaterial::SaveAs(const std::filesystem::path& filepath) const
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("PhysicsMaterial");

	doc.InsertFirstChild(pRoot);

	pRoot->SetAttribute("Density", m_Density);
	pRoot->SetAttribute("Friction", m_Friction);
	pRoot->SetAttribute("Restitution", m_Restitution);

	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);

	tinyxml2::XMLError error = doc.SaveFile(absolutePath.string().c_str());

	return error == tinyxml2::XML_SUCCESS;
}

bool PhysicsMaterial::Save() const
{
	return SaveAs(m_Filepath);
}