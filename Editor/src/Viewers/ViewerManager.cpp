#include "Engine.h"

#include "ViewerManager.h"
#include "TextureView.h"
#include "ScriptView.h"
#include "StaticMeshView.h"
#include "MaterialView.h"
#include "TilesetView.h"
#include "SpriteSheetView.h"
#include "VisualScriptView.h"
#include "PhysicsMaterialView.h"
#include "FontView.h"
#include "BehaviourTreeView.h"
#include "Scene/SceneManager.h"
#include "AudioView.h"

std::map<std::filesystem::path, std::pair<Ref<View>, bool*>> s_AssetViewers;
Ref<FileWatcher> s_FileWatcher = nullptr;

static const char* TextExtensions[] =
{
  ".txt", ".ini", ".lua", ".cpp", ".h", ".c", ".hlsl", ".fx",
  ".glsl", ".frag", ".vert", ".tesc", ".tese", ".geom", ".comp",
  ".json"
};

static const char* ImageExtensions[] =
{
  ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"
};

static const char* MeshExtensions[] =
{
  ".staticmesh"
  //".3ds", ".blend", ".dae", ".fbx", ".obj", ".mesh", ".stl"
};

static const char* AudioExtensions[] =
{
  ".mp3", ".wav", ".flac"
};

static const char* FontExtensions[] =
{
  ".ttf", ".otf"
};

template<typename T>
void OpenAssetViewer(const std::filesystem::path& assetPath)
{
	bool* show = new bool(true);
	Ref<View> layer = CreateRef<T>(show, assetPath);
	s_AssetViewers[assetPath] = std::make_pair(layer, show);
	Application::GetLayerStack().AddOverlay(layer);
}

void ViewerManager::OpenViewer(const std::filesystem::path& assetPath)
{
	if (s_FileWatcher == nullptr)
	{
		s_FileWatcher = CreateRef<FileWatcher>(std::chrono::seconds(1));
		s_FileWatcher->SetPathToWatch(Application::GetOpenDocumentDirectory());
		s_FileWatcher->Start([=](std::string path, FileStatus status)
			{
				if (status == FileStatus::Erased)
				{
					if (s_AssetViewers.find(path) != s_AssetViewers.end())
						CloseViewer(path);
				}
			});
	}

	//check if any old ones can be closed
	if (s_AssetViewers.size() > MAX_ASSET_VIEWERS)
	{
		auto iter = s_AssetViewers.begin();

		for (; iter != s_AssetViewers.end();)
		{
			if (!*iter->second.second)
			{
				Application::GetLayerStack().RemoveOverlay(iter->second.first);
				iter = s_AssetViewers.erase(iter);
			}
			else
				++iter;
		}
	}

	if (s_AssetViewers.find(assetPath) != s_AssetViewers.end())
	{
		*s_AssetViewers.at(assetPath).second = true;
		ImGui::SetWindowFocus(s_AssetViewers.at(assetPath).first->GetWindowName().c_str());
		return;
	}

	switch (GetFileType(assetPath))
	{
	case FileType::SCENE:			SceneManager::ChangeScene(assetPath);				return;
	case FileType::IMAGE:			OpenAssetViewer<TextureView>(assetPath);			return;
	case FileType::MESH:			OpenAssetViewer<StaticMeshView>(assetPath);			return;
	case FileType::SCRIPT:
	case FileType::TEXT:			OpenAssetViewer<ScriptView>(assetPath);				return;
	case FileType::MATERIAL:		OpenAssetViewer<MaterialView>(assetPath);			return;
	case FileType::AUDIO:			OpenAssetViewer<AudioView>(assetPath);				return;
	case FileType::TILESET:			OpenAssetViewer<TilesetView>(assetPath);			return;
	case FileType::SPRITESHEET:		OpenAssetViewer<SpriteSheetView>(assetPath);		return;
	case FileType::VISUALSCRIPT:	OpenAssetViewer<VisualScriptView>(assetPath);		return;
	case FileType::PHYSICSMATERIAL:	OpenAssetViewer<PhysicsMaterialView>(assetPath);	return;
	case FileType::FONT:			OpenAssetViewer<FontView>(assetPath);				return;
	case FileType::BEHAVIOURTREE:	OpenAssetViewer<BehaviourTreeView>(assetPath);			return;
	default: return;
	}
}

void ViewerManager::CloseViewer(const std::filesystem::path& assetPath)
{
	Application::GetLayerStack().RemoveOverlay(s_AssetViewers.at(assetPath).first);
	s_AssetViewers.erase(assetPath);
}

void ViewerManager::CloseAll()
{
	while (s_AssetViewers.size() > 0)
	{
		CloseViewer(s_AssetViewers.begin()->first);
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

	if (strcmp(ext, ".visualscript") == 0)
	{
		return FileType::VISUALSCRIPT;
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

	if (strcmp(ext, ".tileset") == 0)
	{
		return FileType::TILESET;
	}

	if (strcmp(ext, ".spritesheet") == 0)
	{
		return FileType::SPRITESHEET;
	}

	if (strcmp(ext, ".physicsmaterial") == 0)
	{
		return FileType::PHYSICSMATERIAL;
	}

	for (const char* knownExt : FontExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			return FileType::FONT;
		}
	}

	if (strcmp(ext, ".behaviourtree") == 0)
	{
		return FileType::BEHAVIOURTREE;
	}
	return FileType::UNKNOWN;
}

std::vector<std::string> ViewerManager::GetExtensions(FileType fileType)
{
	std::vector<std::string> extensions;
	switch (fileType)
	{
	case FileType::UNKNOWN:
		return extensions;
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
	case FileType::VISUALSCRIPT:
	{
		extensions.push_back(".visualscript");
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
		extensions.push_back(".tileset");
		break;
	}
	case FileType::SPRITESHEET:
	{
		extensions.push_back(".spritesheet");
		break;
	}
	case FileType::PHYSICSMATERIAL:
		extensions.push_back(".physicsmaterial");
		break;
	case FileType::FONT:
		for (const char* ext : FontExtensions)
		{
			extensions.push_back(ext);
		}
		break;
	case FileType::BEHAVIOURTREE:
	{
		extensions.push_back(".behaviourtree");
		break;
	}
    case FileType::STATEMACHINE:
    {
        extensions.push_back(".statemachine");
    }
	}
	return extensions;
}

void ViewerManager::SaveAll()
{
	for (auto&& [path, viewer] : s_AssetViewers)
	{
		if (Ref<ISaveable> saveableView = std::dynamic_pointer_cast<ISaveable>(viewer.first))
			if (saveableView->NeedsSaving())
				saveableView->Save();
	}
}
