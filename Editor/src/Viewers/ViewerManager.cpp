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

static const char* AudioExtensions[] =
{
	".ogg", ".mp3", ".wav"
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
	case FileType::AUDIO:
	{
		CLIENT_WARN("No audio viewer implemented");
	}
	case FileType::TILESET:
	{
		//TODO: implement tileset viewer
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

	if (strcmp(ext, ".lua") == 0)
	{
		return FileType::SCRIPT;
	}

	if (strcmp(ext, ".material") == 0)
	{
		return FileType::MATERIAL;
	}

	for (const char* knownExt : AudioExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			return FileType::AUDIO;
		}
	}

	if (strcmp(ext, ".tsx") == 0)
	{
		return FileType::TILESET;
	}

	return FileType::UNKNOWN;
}

std::vector<std::string> ViewerManager::GetExtensions(FileType fileType)
{
	std::vector<std::string> extensions;
	switch (fileType)
	{
	case FileType::TEXT:
	{
		for (const char* ext : TextExtensions)
		{
			extensions.push_back(ext);
		}
		break;
	}
	case FileType::IMAGE:
	{
		for (const char* ext : ImageExtensions)
		{
			extensions.push_back(ext);
		}
		break;
	}
	case FileType::MESH:
	{
		for (const char* ext : MeshExtensions)
		{
			extensions.push_back(ext);
		}
		break;
	}
	case FileType::SCENE:
	{
		extensions.push_back(".scene");
		break;
	}
	case FileType::SCRIPT:
	{
		extensions.push_back(".lua");
		break;
	}
	case FileType::AUDIO:
	{
		for (const char* ext : AudioExtensions)
		{
			extensions.push_back(ext);
		}
		break;
	}
	case FileType::MATERIAL:
	{
		extensions.push_back(".material");
		break;
	}
	case FileType::TILESET:
	{
		extensions.push_back(".tsx");
		break;
	}
	}
	return extensions;
}

void ViewerManager::SaveAll()
{
	for (auto&& [path, viewer] : s_AssetViewers)
	{
		if (ISaveable* saveableView = dynamic_cast<ISaveable*>(viewer.first))
			saveableView->Save();
	}
}
