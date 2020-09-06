#pragma once

#include "include.h"

#define MAX_ASSET_VIEWERS 2

class ViewerManager
{
public:
	static void OpenViewer(const std::filesystem::path& assetPath);

private:

	static std::map<std::filesystem::path, std::pair<Layer*, bool*>> s_AssetViewers;
};
