#pragma once
#include <filesystem>
#include <unordered_set>

#include <Core/Layer.h>
#include <ProjectData.h>

class AssetPacker : public Layer
{
public:
	AssetPacker(bool* show, const std::filesystem::path& projectDirectory, const std::filesystem::path& exportDirectory);

	void OnImGuiRender();

private:
	void DiscoverAssets();
	void PackAssets();
	void ExportGame();

	bool m_Show = true;

	std::filesystem::path m_ProjectDirectory;
	std::filesystem::path m_ExportDirectory;
	std::filesystem::path m_GameName;
	std::vector<std::filesystem::path> m_DiscoveredAssets;
	std::unordered_set<std::filesystem::path> m_SelectedAssets;

	ProjectData m_Data;
};
