#include "stdafx.h"
#include "AssetManager.h"

AssetManager* AssetManager::s_Instance = nullptr;


AssetManager& AssetManager::Get()
{
	if (!s_Instance)
	{
		s_Instance = new AssetManager();
	}
	return *s_Instance;
}
