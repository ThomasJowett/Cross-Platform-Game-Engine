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
	SPRITESHEET,
	PHYSICSMATERIAL,
	FONT,
	BEHAVIOURTREE,
	STATEMACHINE
};

class View : public Layer
{
public:
	View(std::string name)
		:Layer(name) {}
	virtual ~View() = default;
	const std::string& GetWindowName() { return m_WindowName; }
protected:
	std::string m_WindowName;
};

class ViewerManager
{
public:

	static void OpenViewer(const std::filesystem::path& assetPath);
	static void CloseViewer(const std::filesystem::path& assetPath);
	static void CloseAll();

	static FileType GetFileType(const std::filesystem::path& assetPath);

	static std::vector<std::string> GetExtensions(FileType fileType);

	static void SaveAll();
};

