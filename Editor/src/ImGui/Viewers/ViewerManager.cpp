#include "stdafx.h"
#include "ViewerManager.h"

#include "ImGuiTextureView.h"
#include "ImGuiScriptView.h"

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

void ViewerManager::OpenViewer(std::filesystem::path assetPath)
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

	auto ext = assetPath.extension();
	for (const char* knownExt : TextExtensions)
	{
		if (ext == knownExt)
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
		if (ext == knownExt)
		{
			bool* show = new bool(true);
			Layer* layer = new ImGuiTextureView(show, assetPath);
			s_AssetViewers[assetPath] = std::make_pair(layer, show);
			Application::Get().AddOverlay(layer);
			return;
		}
	}
}
