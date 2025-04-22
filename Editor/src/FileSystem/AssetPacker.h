#pragma once
#include <filesystem>
#include <unordered_set>

#include <Core/Layer.h>

class AssetPacker : public Layer
{
public:
	AssetPacker(bool* show, const std::filesystem::path& projectDirectory);

	void OnImGuiRender();

private:
	void DiscoverAssets();
	void PackAssets();

	bool m_Show = true;

	std::filesystem::path m_ProjectDirectory;
	std::vector<std::filesystem::path> m_DiscoveredAssets;
	std::unordered_set<std::filesystem::path> m_SelectedAssets;
};
