#pragma once
#include <filesystem>
#include <unordered_set>

#include <Core/Layer.h>
#include <ProjectData.h>

class AssetPacker : public Layer
{
	enum Stage
	{
		DiscoveringAssets,
		PackingAssets,
		ExportingGame,
		Done
	};

	struct AssetNode {
		std::string name;
		std::filesystem::path fullPath;
		bool isDirectory = false;
		std::vector<Ref<AssetNode>> children;
	};
	
public:
	AssetPacker(bool* show, const std::filesystem::path& projectDirectory, const std::filesystem::path& exportDirectory);

	void OnImGuiRender();

private:
	void DiscoverAssets();
	void AddAssetToTree(const std::filesystem::path& assetPath, Ref<AssetNode> root);
	void PackAssets();
	void ExportGame();

	void DrawAssetTree(Ref<AssetNode> node);

	bool m_Show = true;

	std::filesystem::path m_ProjectDirectory;
	std::filesystem::path m_ExportDirectory;
	std::filesystem::path m_GameName;
	std::vector<std::filesystem::path> m_DiscoveredAssets;
	std::unordered_set<std::filesystem::path> m_SelectedAssets;

	ProjectData m_Data;

	std::atomic<float> m_Progress = 0.0f;
	std::atomic<Stage> m_CurrentStage = Stage::DiscoveringAssets;
	std::thread m_PackThread;

	Ref<AssetNode> m_AssetTree;
};
