#include "stdafx.h"
#include "AssetManager.h"

AssetManager* AssetManager::s_Instance = nullptr;
std::mutex AssetManager::s_Mutex = std::mutex();

Ref<Texture2D> AssetManager::GetTexture(const std::filesystem::path& filepath)
{
	return Get().GetTextureImpl(filepath);
}

Ref<Tileset> AssetManager::GetTileset(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetTilesetImpl(filepath);
}

Ref<Material> AssetManager::GetMaterial(const std::filesystem::path& filepath)
{
	return Get().GetMaterialImpl(filepath);
}

Ref<StaticMesh> AssetManager::GetStaticMesh(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetStaticMeshImpl(filepath);
}

Ref<PhysicsMaterial> AssetManager::GetPhysicsMaterial(const std::filesystem::path& filepath)
{
	std::lock_guard lock(s_Mutex);
	return Get().GetPhysicsMaterialImpl(filepath);
}

AssetManager& AssetManager::Get()
{
	if (!s_Instance)
	{
		s_Instance = new AssetManager();
	}
	return *s_Instance;
}

Ref<Texture2D> AssetManager::GetTextureImpl(const std::filesystem::path& filepath)
{
	return m_Textures.Load(filepath);
}

Ref<Tileset> AssetManager::GetTilesetImpl(const std::filesystem::path& filepath)
{
	return m_Tilesets.Load(filepath);
}

Ref<Material> AssetManager::GetMaterialImpl(const std::filesystem::path& filepath)
{
	return m_Materials.Load(filepath);
}

Ref<StaticMesh> AssetManager::GetStaticMeshImpl(const std::filesystem::path& filepath)
{
	return m_StaticMeshes.Load(filepath);
}

Ref<PhysicsMaterial> AssetManager::GetPhysicsMaterialImpl(const std::filesystem::path& filepath)
{
	return m_PhysicsMaterials.Load(filepath);
}