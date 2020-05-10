#include "stdafx.h"
#include "ViewerManager.h"

#include "ImGuiTextureView.h"
#include "ImGuiScriptView.h"

//std::unordered_map<std::filesystem::path, Ref<Layer>> ViewerManager::s_AssetViewers;

Ref<Layer> ViewerManager::GetAssetViewer(std::filesystem::path assetPath)
{
	//if (s_AssetViewers.find(assetPath) != s_AssetViewers.end())
	//	return s_AssetViewers.at(assetPath);

	return Ref<Layer>();
}

void ViewerManager::OpenViewer(Ref<Layer> viewer)
{
}

void ViewerManager::CloseViewer(Ref<Layer> viewer)
{
}
