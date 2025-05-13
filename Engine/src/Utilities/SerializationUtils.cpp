#include "stdafx.h"
#include "SerializationUtils.h"
#include "FileUtils.h"
#include "Scene/AssetManager.h"

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Vector2f& vec2)
{
	pElement->SetAttribute("X", vec2.x);
	pElement->SetAttribute("Y", vec2.y);
}

void SerializationUtils::Decode(tinyxml2::XMLElement const* pElement, Vector2f& vec2)
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

void SerializationUtils::Decode(tinyxml2::XMLElement const* pElement, Vector3f& vec3)
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

void SerializationUtils::Decode(tinyxml2::XMLElement const* pElement, Colour& colour)
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

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const std::filesystem::path& filepath)
{
	if (!filepath.empty())
	{
		std::filesystem::path finalPath = filepath;
		finalPath.make_preferred();
		std::string finalPathString = finalPath.string();
		std::replace(finalPathString.begin(), finalPathString.end(), '\\', '/');
		pElement->SetAttribute("Filepath", finalPathString.c_str());
	}
}

void SerializationUtils::Decode(tinyxml2::XMLElement const* pElement, std::filesystem::path& filepath)
{
	if (pElement)
	{
		const char* relativePath = pElement->Attribute("Filepath");
		if (relativePath)
		{
			filepath = relativePath;
		}
	}
}

void SerializationUtils::Encode(tinyxml2::XMLElement* pElement, const Ref<Texture2D>& texture)
{
	if (pElement)
	{
		Encode(pElement, texture->GetFilepath());
		pElement->SetAttribute("FilterMethod", (int)texture->GetFilterMethod());
		pElement->SetAttribute("WrapMethod", (int)texture->GetWrapMethod());
	}
}

void SerializationUtils::Decode(tinyxml2::XMLElement const* pElement, Ref<Texture2D>& texture)
{
	if (pElement)
	{
		std::filesystem::path filepath;
		Decode(pElement, filepath);
		texture = AssetManager::GetTexture(filepath);
		if (texture)
		{
			int filterMethod = pElement->IntAttribute("FilterMethod", (int)Texture::FilterMethod::Linear);
			texture->SetFilterMethod((Texture::FilterMethod)filterMethod);

			int wrapMethod = pElement->IntAttribute("WrapMethod", (int)Texture::WrapMethod::Repeat);
			texture->SetWrapMethod((Texture::WrapMethod)wrapMethod);
		}
	}
}

std::string SerializationUtils::RelativePath(const std::filesystem::path& path)
{
	std::string relativePath = FileUtils::RelativePath(path, Application::GetOpenDocumentDirectory()).make_preferred().string();
	std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
	return relativePath;
}

std::filesystem::path SerializationUtils::AbsolutePath(const char* path)
{
	if (path != nullptr)
		return std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path);
	else
		return std::filesystem::path();
}