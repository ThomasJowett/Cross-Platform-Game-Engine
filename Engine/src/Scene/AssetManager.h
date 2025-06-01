#pragma once

#include "Core/core.h"
#include "Core/Application.h"
#include "Core/Asset.h"
#include "Logging/Instrumentor.h"
#include "Asset/Texture.h"
#include "Core/Factory.h"
#include "Core/VirtualFileSystem.h"
#include "Utilities/FileUtils.h"

class AssetManager
{
public:
	static void LoadBundle(const void* zipData, size_t zipSize)
	{
		PROFILE_FUNCTION();
		AssetManager::Get().m_VFS = CreateRef<VirtualFileSystem>();
		AssetManager::Get().m_VFS->Mount(zipData, zipSize);
		ENGINE_INFO("Asset Bundle Loaded");
	}

	static bool HasBundle()
	{
		return (bool)AssetManager::Get().m_VFS;
	}

	static bool FileExistsInBundle(const std::filesystem::path& name)
	{
		PROFILE_FUNCTION();
		if (s_Instance->m_VFS) {
			return s_Instance->m_VFS->Exists(name);
		}
		return false;
	}

	static bool GetFileData(const std::filesystem::path& name, std::vector<uint8_t>& data)
	{
		PROFILE_FUNCTION();
		if (s_Instance->m_VFS) {
			return s_Instance->m_VFS->ReadFile(name, data);
		}
		return false;
	}

	static mz_zip_archive* GetBundleArchive()
	{
		PROFILE_FUNCTION();
		if (AssetManager::Get().m_VFS) {
			return &AssetManager::Get().m_VFS->GetArchive();
		}
		return nullptr;
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
		return AssetManager::Get().m_Textures.Load(filepath, AssetManager::Get().m_VFS);
	}

	static void CleanUp()
	{
		PROFILE_FUNCTION();
		AssetManager::Get().m_Assets.CleanUnused();
		AssetManager::Get().m_Textures.Clear();
	}

	static void Init(const std::filesystem::path& directory)
	{
		PROFILE_FUNCTION();

		AssetManager::Get().m_FileWatcher.SetPathToWatch(directory);

		AssetManager::Get().m_FileWatcher.SetPathToWatch(directory);
		AssetManager::Get().m_FileWatcher.Stop();
		AssetManager::Get().m_FileWatcher.Start([=](std::string path, FileStatus status)
			{
				std::filesystem::path relativePath = FileUtils::RelativePath(path, Application::GetOpenDocumentDirectory());
				switch (status)
				{
				case FileStatus::Created:
					break;
				case FileStatus::Modified:
					if (AssetManager::Get().m_Assets.Exists(relativePath))
					{
						ENGINE_DEBUG("Reloading asset {0}", path);
						AssetManager::Get().m_Assets.Get(relativePath)->Reload();
					}
					else if (AssetManager::Get().m_Textures.Exists(relativePath))
					{
						ENGINE_DEBUG("Reloading texture {0}", path);
						AssetManager::Get().m_Textures.Get(relativePath)->Reload();
					}
					break;
				case FileStatus::Erased:
					if (AssetManager::Get().m_Assets.Exists(relativePath))
					{
						ENGINE_ERROR("An asset in use has been deleted! {0}", path);
						AssetManager::Get().m_Assets.Remove(relativePath);
					}
					else if (AssetManager::Get().m_Textures.Exists(relativePath))
					{
						ENGINE_ERROR("A texture in use has been deleted! {0}", path);
						AssetManager::Get().m_Textures.Get(relativePath)->Reload();
					}

					break;
				default:
					break;
				}
			});
	}

	static void Shutdown()
	{
		PROFILE_FUNCTION();
		AssetManager::Get().m_Assets.Clear();
		AssetManager::Get().m_Textures.Clear();
		if(AssetManager::Get().m_VFS)
			AssetManager::Get().m_VFS->Unmount();
		AssetManager::Get().m_VFS.reset();
	}

private:
	AssetManager();
	~AssetManager() = default;
	static AssetManager& Get();

	AssetLibrary m_Assets;
	TextureLibrary2D m_Textures;
	Ref<VirtualFileSystem> m_VFS;

	FileWatcher m_FileWatcher;

	static AssetManager* s_Instance;
};
