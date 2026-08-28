#pragma once

#include <filesystem>
#include <vector>

class SpriteAtlasBuilder
{
public:
	static void EnsureUpToDate();
	static void Rebuild();
	static std::filesystem::path GetManifestPath() { return std::filesystem::path(kOutputDir) / kManifestName; }

private:
	static std::vector<std::filesystem::path> DiscoverSpriteTextures();
	static bool IsStale(const std::vector<std::filesystem::path>& currentSources);
	static void DoBuild(const std::vector<std::filesystem::path>& sources);
	static uint32_t GetConfiguredPageSize();

	static constexpr const char* kOutputDir = "Generated/SpriteAtlas";
	static constexpr const char* kManifestName = "Manifest.atlas";
};
