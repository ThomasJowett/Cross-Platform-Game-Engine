#pragma once

#define MAX_ASSET_VIEWERS 2

#include <map>

#include "Core/Layer.h"

enum class FileType
{
	UNKNOWN,
	TEXT,
	IMAGE,
	MESH,
	SCENE,
	SCRIPT,
	AUDIO,
	MATERIAL
};

class ViewerManager
{
public:
	
	static void OpenViewer(const std::filesystem::path& assetPath);

	static FileType GetFileType(const std::filesystem::path& assetPath);

private:

	static std::map<std::filesystem::path, std::pair<Layer*, bool*>> s_AssetViewers;
};
