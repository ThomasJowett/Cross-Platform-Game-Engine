#include "stdafx.h"
#include "AssetManager.h"

AssetManager* AssetManager::s_Instance = nullptr;
std::mutex AssetManager::s_Mutex = std::mutex();

Ref<Tileset> AssetManager::GetTileset(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetTilesetImpl(filepath);
}

Ref<Material> AssetManager::GetMaterial(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetMaterialImpl(filepath);
}

Ref<Mesh> AssetManager::GetMesh(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetMeshImpl(filepath);
}

AssetManager& AssetManager::Get()
{
	if (!s_Instance)
	{
		s_Instance = new AssetManager();
	}
	return *s_Instance;
}

Ref<Tileset> AssetManager::GetTilesetImpl(const std::filesystem::path& filepath)
{
	return m_Tilesets.Load(filepath);
}

Ref<Material> AssetManager::GetMaterialImpl(const std::filesystem::path& filepath)
{
	return m_Materials.Load(filepath);
}

Ref<Mesh> AssetManager::GetMeshImpl(const std::filesystem::path& filepath)
{
	return m_Meshes.Load(filepath);
}
