#pragma once

#define MAX_ASSET_VIEWERS 2

#include <map>

class ViewerManager
{
public:
	static void OpenViewer(const std::filesystem::path& assetPath);

private:

	static std::map<std::filesystem::path, std::pair<Layer*, bool*>> s_AssetViewers;
};
