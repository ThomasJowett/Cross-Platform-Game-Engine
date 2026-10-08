#pragma once

#include "math/Vector2f.h"
#include "math/Vector3f.h"

#include "Asset/Material.h"

#include "Core/Colour.h"
#include "Core/Asset.h"

#include "TinyXml2/tinyxml2.h"
#include "Scene/AssetManager.h"


namespace SerializationUtils
{
void Encode(tinyxml2::XMLElement* pElement, const Vector2f& vec2);

void Decode(tinyxml2::XMLElement const* pElement, Vector2f& vec2);

void Encode(tinyxml2::XMLElement* pElement, const Vector3f& vec3);

void Decode(tinyxml2::XMLElement const* pElement, Vector3f& vec3);

void Encode(tinyxml2::XMLElement* pElement, const Colour& colour);

void Decode(tinyxml2::XMLElement const* pElement, Colour& colour);

void Encode(tinyxml2::XMLElement* pElement, const std::filesystem::path& filepath);

void Decode(tinyxml2::XMLElement const* pElement, std::filesystem::path& filepath);

void Encode(tinyxml2::XMLElement* pElement, const Ref<Texture2D>& texture);

void Decode(tinyxml2::XMLElement const* pElement, Ref<Texture2D>& texture);

// A texture reference plus the filter/wrap its owner samples it with
void Encode(tinyxml2::XMLElement* pElement, const Ref<Texture2D>& texture, Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod);

void Decode(tinyxml2::XMLElement const* pElement, Ref<Texture2D>& texture, Texture::FilterMethod& filterMethod, Texture::WrapMethod& wrapMethod);

std::string RelativePath(const std::filesystem::path& path);
std::filesystem::path AbsolutePath(const char* path);

template<typename Archive>
void SaveAssetToArchive(Archive& archive, const Ref<Asset>& asset)
{
	std::string relativePath;
	if (asset && !asset->GetFilepath().empty())
	{
		relativePath = asset->GetFilepath().string();
	}
	archive(relativePath);
}

template<typename Archive, typename Asset>
void LoadAssetFromArchive(Archive& archive, Ref<Asset>& asset)
{
	std::string relativePath;
	archive(relativePath);
	if (!relativePath.empty())
	{
		asset = AssetManager::GetAsset<Asset>(relativePath);
	}
	else
	{
		asset.reset();
	}
}

template<typename Archive>
void SaveTextureToArchive(Archive& archive, const Ref<Texture2D>& texture, Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod)
{
	std::string relativePath;
	if (texture && !texture->GetFilepath().empty())
	{
		relativePath = texture->GetFilepath().string();
	}
	archive(relativePath);
	archive((int)filterMethod);
	archive((int)wrapMethod);
}

template<typename Archive>
void LoadTextureFromArchive(Archive& archive, Ref<Texture2D>& texture, Texture::FilterMethod& filterMethod, Texture::WrapMethod& wrapMethod)
{
	std::string relativePath;
	archive(relativePath);
	int filter, wrap;
	archive(filter);
	archive(wrap);
	filterMethod = (Texture::FilterMethod)filter;
	wrapMethod = (Texture::WrapMethod)wrap;
	texture = relativePath.empty() ? nullptr : AssetManager::GetTexture(relativePath);
}
}