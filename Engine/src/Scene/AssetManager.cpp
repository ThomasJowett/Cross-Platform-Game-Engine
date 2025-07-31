#include "stdafx.h"
#include "AssetManager.h"

AssetManager* AssetManager::s_Instance = nullptr;


AssetManager::AssetManager()
	:m_FileWatcher(std::chrono::milliseconds(2000))
{
}

AssetManager& AssetManager::Get()
{
	if (!s_Instance)
	{
		s_Instance = new AssetManager();
	}
	return *s_Instance;
}
