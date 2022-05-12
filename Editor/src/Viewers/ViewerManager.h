#pragma once

#define MAX_ASSET_VIEWERS 2

#include <map>
#include <filesystem>

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
	MATERIAL,
	TILESET
};

class ViewerManager
{
public:
	
	static void OpenViewer(const std::filesystem::path& assetPath);

	static FileType GetFileType(const std::filesystem::path& assetPath);

	static std::vector<std::string> GetExtensions(FileType fileType);

	static void SaveAll();

private:
	static std::map<std::filesystem::path, std::pair<Layer*, bool*>> s_AssetViewers;
};
