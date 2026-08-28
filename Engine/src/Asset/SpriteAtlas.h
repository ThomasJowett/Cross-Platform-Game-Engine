#pragma once

#include "Core/Asset.h"
#include "Asset/Texture.h"
#include "math/Vector2f.h"

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace tinyxml2
{
class XMLDocument;
}

class SpriteAtlas : public Asset
{
public:
	struct Region
	{
		uint32_t page = 0;
		Vector2f uvMin;
		Vector2f uvMax;
	};

	struct SourceRecord
	{
		std::filesystem::path path;
		int64_t mtime = 0;
		uintmax_t size = 0;
	};

	SpriteAtlas() = default;
	SpriteAtlas(const std::filesystem::path& filepath);
	SpriteAtlas(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;

	const Region* GetRegion(const std::filesystem::path& path) const;

	Ref<Texture2D> GetPage(uint32_t index) const;
	uint32_t GetPageCount() const { return (uint32_t)m_PagePaths.size(); }
	const std::vector<SourceRecord>& GetSources() const { return m_Sources; }

	static Ref<SpriteAtlas> Build(const std::vector<std::filesystem::path>& sourceImagePaths, const std::filesystem::path& outputDir, uint32_t pageSize);

private:
	bool LoadXML(tinyxml2::XMLDocument* doc, const std::filesystem::path& manifestDir);

	std::vector<std::filesystem::path> m_PagePaths;
	mutable std::vector<Ref<Texture2D>> m_PageCache;
	std::vector<SourceRecord> m_Sources;
	std::unordered_map<std::filesystem::path, Region> m_Regions;
};
