#include "Engine.h"

#include "ViewerManager.h"
#include "TextureView.h"
#include "ScriptView.h"
#include "StaticMeshView.h"
#include "MaterialView.h"
#include "Scene/SceneManager.h"

std::map<std::filesystem::path, std::pair<Layer*, bool*>> ViewerManager::s_AssetViewers;

static const char* TextExtensions[] =
{
	".txt", ".ini", ".lua", ".cpp", ".h", ".c", ".hlsl", ".fx",
	".glsl", ".frag", ".vert", ".tesc", ".tese", ".geom", ".comp",
	".json"
};

static const char* ImageExtensions[] =
{
	".png", ".jpg", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"
};

static const char* MeshExtensions[] =
{
	".staticmesh"
	//".3ds", ".blend", ".dae", ".fbx", ".obj", ".mesh", ".stl"
};

void ViewerManager::OpenViewer(const std::filesystem::path& assetPath)
{
	//check if any old ones can be closed
	if (s_AssetViewers.size() > MAX_ASSET_VIEWERS)
	{
		auto iter = s_AssetViewers.begin();

		for (; iter != s_AssetViewers.end();)
		{
			if (!*iter->second.second)
			{
				Application::Get().RemoveOverlay(iter->second.first);
				iter = s_AssetViewers.erase(iter);
			}
			else
				++iter;
		}
	}

	if (s_AssetViewers.find(assetPath) != s_AssetViewers.end())
	{
		*s_AssetViewers.at(assetPath).second = true;
		return;
	}

	switch (GetFileType(assetPath))
	{
	case FileType::IMAGE:
	{
		bool* show = new bool(true);
		Layer* layer = new TextureView(show, assetPath);
		s_AssetViewers[assetPath] = std::make_pair(layer, show);
		Application::Get().AddOverlay(layer);
		return;
	}
	case FileType::MESH:
	{
		bool* show = new bool(true);
		Layer* layer = new StaticMeshView(show, assetPath);
		s_AssetViewers[assetPath] = std::make_pair(layer, show);
		Application::Get().AddOverlay(layer);
		return;
	}
	case FileType::SCENE:
	{
		SceneManager::ChangeScene(assetPath);
	}
	return;
	[[fallthrough]];
	case FileType::SCRIPT:
	case FileType::TEXT:
	{
		bool* show = new bool(true);
		Layer* layer = new ScriptView(show, assetPath);
		s_AssetViewers[assetPath] = std::make_pair(layer, show);
		Application::Get().AddOverlay(layer);
		return;
	}
	case FileType::MATERIAL:
	{
		bool* show = new bool(true);
		Layer* layer = new MaterialView(show, assetPath);
		s_AssetViewers[assetPath] = std::make_pair(layer, show);
		Application::Get().AddOverlay(layer);
		return;
	}
	default:
		return;
	}
}

FileType ViewerManager::GetFileType(const std::filesystem::path& assetPath)
{
	std::string extString = assetPath.extension().string();

	std::transform(extString.begin(), extString.end(), extString.begin(), ::tolower);

	const char* ext = extString.c_str();

	for (const char* knownExt : TextExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			return FileType::TEXT;
		}
	}

	for (const char* knownExt : ImageExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			return FileType::IMAGE;
		}
	}

	for (const char* knownExt : MeshExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			return FileType::MESH;
		}
	}

	if (strcmp(ext, ".scene") == 0)
	{
		return FileType::SCENE;
	}

	if (strcmp(ext, ".cs") == 0)
	{
		return FileType::SCRIPT;
	}

	if (strcmp(ext, ".material") == 0)
	{
		return FileType::MATERIAL;
	}

	return FileType::UNKNOWN;
}
