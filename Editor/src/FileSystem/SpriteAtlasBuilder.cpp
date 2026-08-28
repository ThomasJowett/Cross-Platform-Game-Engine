#include "SpriteAtlasBuilder.h"

#include "Directory.h"
#include "Viewers/ViewerManager.h"
#include "ProjectData.h"

#include "Core/Application.h"
#include "Asset/SpriteAtlas.h"
#include "Scene/AssetManager.h"
#include "Renderer/Renderer2D.h"
#include "TinyXml2/tinyxml2.h"
#include "Logging/Logger.h"

#include "cereal/archives/json.hpp"

#include <unordered_map>

void SpriteAtlasBuilder::EnsureUpToDate()
{
	std::vector<std::filesystem::path> sources = DiscoverSpriteTextures();
	if (sources.empty())
	{
		Renderer2D::SetSpriteAtlas(nullptr);
		return;
	}

	if (IsStale(sources))
	{
		DoBuild(sources);
		return;
	}

	if (Ref<SpriteAtlas> atlas = AssetManager::GetAsset<SpriteAtlas>(std::filesystem::path(kOutputDir) / kManifestName))
		Renderer2D::SetSpriteAtlas(atlas);
	else
		DoBuild(sources);
}

void SpriteAtlasBuilder::Rebuild()
{
	DoBuild(DiscoverSpriteTextures());
}

std::vector<std::filesystem::path> SpriteAtlasBuilder::DiscoverSpriteTextures()
{
	std::unordered_map<std::string, bool> pathWantsAtlas;

	for (const std::filesystem::path& sceneFile : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::SCENE)))
	{
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(sceneFile.string().c_str()) != tinyxml2::XML_SUCCESS)
			continue;

		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");
		if (!pRoot)
			continue;

		for (tinyxml2::XMLElement* pEntity = pRoot->FirstChildElement("Entity"); pEntity; pEntity = pEntity->NextSiblingElement("Entity"))
		{
			tinyxml2::XMLElement* pSprite = pEntity->FirstChildElement("Sprite");
			if (!pSprite)
				continue;

			tinyxml2::XMLElement* pTexture = pSprite->FirstChildElement("Texture");
			if (!pTexture)
				continue;

			const char* filepath = pTexture->Attribute("Filepath");
			if (!filepath)
				continue;

			float tilingFactor = 1.0f;
			pSprite->QueryFloatAttribute("TilingFactor", &tilingFactor);

			bool& wantsAtlas = pathWantsAtlas[filepath];
			wantsAtlas = wantsAtlas || tilingFactor == 1.0f;
		}
	}

	std::vector<std::filesystem::path> textures;
	for (const auto& [path, wantsAtlas] : pathWantsAtlas)
		if (wantsAtlas)
			textures.push_back(path);

	return textures;
}

bool SpriteAtlasBuilder::IsStale(const std::vector<std::filesystem::path>& currentSources)
{
	std::filesystem::path manifestPath = std::filesystem::path(kOutputDir) / kManifestName;
	std::filesystem::path absoluteManifestPath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / manifestPath);
	if (!std::filesystem::exists(absoluteManifestPath))
		return true;

	SpriteAtlas existing;
	if (!existing.Load(manifestPath))
		return true;

	const std::vector<SpriteAtlas::SourceRecord>& recorded = existing.GetSources();
	if (recorded.size() != currentSources.size())
		return true;

	std::unordered_map<std::string, const SpriteAtlas::SourceRecord*> recordedByPath;
	for (const SpriteAtlas::SourceRecord& record : recorded)
		recordedByPath[record.path.string()] = &record;

	for (const std::filesystem::path& source : currentSources)
	{
		auto it = recordedByPath.find(source.string());
		if (it == recordedByPath.end())
			return true;

		std::filesystem::path absoluteSourcePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / source);
		std::error_code ec;
		int64_t mtime = std::filesystem::last_write_time(absoluteSourcePath, ec).time_since_epoch().count();
		uintmax_t size = std::filesystem::file_size(absoluteSourcePath, ec);

		if (it->second->mtime != mtime || it->second->size != size)
			return true;
	}

	return false;
}

void SpriteAtlasBuilder::DoBuild(const std::vector<std::filesystem::path>& sources)
{
	if (sources.empty())
	{
		Renderer2D::SetSpriteAtlas(nullptr);
		return;
	}

	Ref<SpriteAtlas> atlas = SpriteAtlas::Build(sources, kOutputDir, GetConfiguredPageSize());
	if (!atlas)
	{
		ENGINE_ERROR("Sprite atlas rebuild failed");
		return;
	}
	Renderer2D::SetSpriteAtlas(atlas);
}

uint32_t SpriteAtlasBuilder::GetConfiguredPageSize()
{
	std::ifstream file(Application::GetOpenDocument());
	if (!file.is_open())
		return 2048;

	ProjectData data;
	cereal::JSONInputArchive input(file);
	input(data);
	return data.spriteAtlasPageSize;
}
