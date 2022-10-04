#pragma once

#ifdef DEBUG
#define MAX_ASSET_VIEWERS 0
#else
#define MAX_ASSET_VIEWERS 10
#endif

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
	VISUALSCRIPT,
	AUDIO,
	MATERIAL,
	TILESET,
	PHYSICSMATERIAL
};

class ViewerManager
{
public:

	static void OpenViewer(const std::filesystem::path& assetPath);
	static void CloseViewer(const std::filesystem::path& assetPath);

	static FileType GetFileType(const std::filesystem::path& assetPath);

	static std::vector<std::string> GetExtensions(FileType fileType);

	static void SaveAll();

private:
	static std::map<std::filesystem::path, std::pair<Layer*, bool*>> s_AssetViewers;
};
