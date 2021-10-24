#include "stdafx.h"
#include "SerializationUtils.h"

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Vector2f& vec2)
{
	pElement->SetAttribute("X", vec2.x);
	pElement->SetAttribute("Y", vec2.y);
}

void SerializationUtils::Decode(tinyxml2::XMLElement* pElement, Vector2f& vec2)
{
	if (pElement)
	{
		pElement->QueryFloatAttribute("X", &vec2.x);
		pElement->QueryFloatAttribute("Y", &vec2.y);
	}
	else
		ENGINE_WARN("Could not find Vector2 element");
}

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Vector3f& vec3)
{
	pElement->SetAttribute("X", vec3.x);
	pElement->SetAttribute("Y", vec3.y);
	pElement->SetAttribute("Z", vec3.z);
}

void SerializationUtils::Decode(tinyxml2::XMLElement* pElement, Vector3f& vec3)
{
	if (pElement)
	{
		pElement->QueryFloatAttribute("X", &vec3.x);
		pElement->QueryFloatAttribute("Y", &vec3.y);
		pElement->QueryFloatAttribute("Z", &vec3.z);
	}
	else
		ENGINE_WARN("Could not find Vector3 element");
}

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Colour& colour)
{
	pElement->SetAttribute("R", colour.r);
	pElement->SetAttribute("G", colour.g);
	pElement->SetAttribute("B", colour.b);
	pElement->SetAttribute("A", colour.a);

}

void SerializationUtils::Decode(tinyxml2::XMLElement* pElement, Colour& colour)
{
	if (pElement)
	{
		pElement->QueryFloatAttribute("R", &colour.r);
		pElement->QueryFloatAttribute("G", &colour.g);
		pElement->QueryFloatAttribute("B", &colour.b);
		pElement->QueryFloatAttribute("A", &colour.a);
	}
	else
		ENGINE_WARN("Could not find Colour element");
}

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Material& material)
{
	std::string relativePath = FileUtils::relativePath(material.GetFilepath(), Application::GetOpenDocumentDirectory()).string();
	pElement->SetAttribute("Filepath", relativePath.c_str());
	material.SaveMaterial();
}

void SerializationUtils::Decode(tinyxml2::XMLElement* pElement, Material& material)
{
	if (pElement)
	{
		const char* materialFilepathChar = pElement->Attribute("Filepath");

		if (materialFilepathChar)
		{
			std::string materialFilepathStr(materialFilepathChar);

			if (!materialFilepathStr.empty())
			{
				std::filesystem::path materailfilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / materialFilepathStr);
				material.LoadMaterial(materailfilepath);
			}
		}
	}
	else
		ENGINE_WARN("Could not find Material node");
}
