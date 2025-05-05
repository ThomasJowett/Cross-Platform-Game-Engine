#pragma once

#include "Core/core.h"
#include "Core/Application.h"
#include "Core/Asset.h"
#include "Logging/Instrumentor.h"
#include "Asset/Texture.h"
#include "Core/Factory.h"
#include "Core/VirtualFileSystem.h"

class AssetManager
{
public:
	static void LoadBundle(const void* zipData, size_t zipSize)
	{
		PROFILE_FUNCTION();
		s_Instance->m_VFS = CreateRef<VirtualFileSystem>();
		s_Instance->m_VFS->Mount(zipData, zipSize);
	}
	template<typename T>
	static Ref<T> GetAsset(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		Ref<Asset> asset = nullptr;
		asset = AssetManager::Get().m_Assets.Load<T>(filepath, AssetManager::Get().m_VFS);
		return std::dynamic_pointer_cast<T>(asset);
	}

	static Ref<Texture2D> GetTexture(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		if(AssetManager::Get().m_VFS)
			return AssetManager::Get().m_Textures.Load(filepath, AssetManager::Get().m_VFS);
		return AssetManager::Get().m_Textures.Load(filepath);
	}

	static void CleanUp()
	{
		AssetManager::Get().m_Assets.CleanUnused();
	}

	static void Shutdown()
	{
		AssetManager::Get().m_Assets.Clear();
		AssetManager::Get().m_Textures.Clear();
		AssetManager::Get().m_VFS->Unmount();
	}

private:
	AssetManager():m_Assets(Application::GetOpenDocumentDirectory()) {};
	~AssetManager() = default;
	static AssetManager& Get();

	AssetLibrary m_Assets;
	TextureLibrary2D m_Textures;
	Ref<VirtualFileSystem> m_VFS;

	static AssetManager* s_Instance;
};
