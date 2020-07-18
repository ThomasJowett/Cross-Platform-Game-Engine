#include "stdafx.h"
#include "ViewerManager.h"

#include "ImGuiTextureView.h"
#include "ImGuiScriptView.h"
#include "ImGuiStaticMeshView.h"

std::map<std::filesystem::path, std::pair<Layer*, bool*>> ViewerManager::s_AssetViewers;

static const char* TextExtensions[] =
{
	".txt", ".ini", ".lua", ".cpp", ".h", ".c", ".hlsl", ".fx",
	".glsl", ".frag", ".vert", ".tesc", ".tese", ".geom", ".comp"
};

static const char* ImageExtensions[] =
{
	".png", ".jpg", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"
};

static const char* MeshExtensions[] =
{
	".3ds", ".blend", ".dae", ".fbx", ".obj", ".mesh", ".stl"
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

	std::string extString = assetPath.extension().string();

	std::transform(extString.begin(), extString.end(), extString.begin(), ::tolower);

	const char* ext = extString.c_str();

	for (const char* knownExt : TextExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			bool* show = new bool(true);
			Layer* layer = new ImGuiScriptView(show, assetPath);
			s_AssetViewers[assetPath] = std::make_pair(layer, show);
			Application::Get().AddOverlay(layer);
			return;
		}
	}

	for (const char* knownExt : ImageExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			bool* show = new bool(true);
			Layer* layer = new ImGuiTextureView(show, assetPath);
			s_AssetViewers[assetPath] = std::make_pair(layer, show);
			Application::Get().AddOverlay(layer);
			return;
		}
	}

	for (const char* knownExt : MeshExtensions)
	{
		if (strcmp(ext, knownExt) == 0)
		{
			bool* show = new bool(true);
			Layer* layer = new ImGuiStaticMeshView(show, assetPath);
			s_AssetViewers[assetPath] = std::make_pair(layer, show);
			Application::Get().AddOverlay(layer);
			return;
		}
	}
}
