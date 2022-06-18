#pragma once

#include "Core/core.h"

#include "Renderer/Asset.h"
#include "Renderer/Tileset.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

#include <mutex>

class AssetManager
{
public:
	static Ref<Tileset> GetTileset(const std::filesystem::path& filepath);
	static Ref<Material> GetMaterial(const std::filesystem::path& filepath);
	static Ref<Mesh> GetMesh(const std::filesystem::path& filepath);

private:
	explicit AssetManager() = default;;
	~AssetManager() = default;
	static AssetManager& Get();

	Ref<Tileset> GetTilesetImpl(const std::filesystem::path& filepath);
	Ref<Material> GetMaterialImpl(const std::filesystem::path& filepath);
	Ref<Mesh> GetMeshImpl(const std::filesystem::path& filepath);

	AssetLibrary<Tileset> m_Tilesets;
	AssetLibrary<Material> m_Materials;
	AssetLibrary<Mesh> m_Meshes;

	static AssetManager* s_Instance;
	static std::mutex s_Mutex;
};
