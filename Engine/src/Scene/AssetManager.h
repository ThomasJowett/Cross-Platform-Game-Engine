#pragma once

#include <fstream>
#include <mutex>
#include <vector>
#include <utility>

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
		// VirtualFileSystem::Mount() (mz_zip_reader_init_mem) keeps a pointer into this data
		// rather than copying it - store our own persistent copy so the VFS stays valid
		// regardless of how long the caller's own buffer lives.
		AssetManager& instance = AssetManager::Get();
		const uint8_t* bytes = static_cast<const uint8_t*>(zipData);
		instance.m_BundleData.assign(bytes, bytes + zipSize);
		instance.m_VFS = CreateRef<VirtualFileSystem>();
		instance.m_VFS->Mount(instance.m_BundleData.data(), instance.m_BundleData.size());
		ENGINE_INFO("Asset Bundle Loaded");
	}

	static bool LoadBundleFromFile(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			ENGINE_ERROR("Failed to open asset bundle: {0}", path.string());
			return false;
		}

		std::streamoff size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<uint8_t> data(static_cast<size_t>(size));
		if (!file.read(reinterpret_cast<char*>(data.data()), size))
		{
			ENGINE_ERROR("Failed to read asset bundle: {0}", path.string());
			return false;
		}

		LoadBundle(data.data(), data.size());
		return true;
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

	// Evicts a stale cache entry after an asset has been renamed/moved on disk - tries
	// both caches since the caller doesn't need to know which type of asset this is.
	// Both AssetLibrary::Remove/TextureLibrary2D::Remove log an error on a cache miss
	// (their only other caller always checks Exists() first), so this must too, or
	// nearly every rename logs spurious "not found" errors for whichever cache (or
	// both, e.g. for a renamed scene, which lives in neither) never had the path.
	static void RemoveAsset(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		if (AssetManager::Get().m_Assets.Exists(filepath))
			AssetManager::Get().m_Assets.Remove(filepath);
		if (AssetManager::Get().m_Textures.Exists(filepath))
			AssetManager::Get().m_Textures.Remove(filepath);
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
		// Just queues the event - Reload()/Remove() touch the GPU, not safe off the main thread.
		AssetManager::Get().m_FileWatcher.Start([=](std::string path, FileStatus status)
			{
				std::filesystem::path relativePath = FileUtils::RelativePath(path, Application::GetOpenDocumentDirectory());
				std::lock_guard<std::mutex> lock(AssetManager::Get().m_PendingEventsMutex);
				AssetManager::Get().m_PendingEvents.push_back({ relativePath, status });
			});
	}

	// Called once per frame from the main thread (Application::Tick).
	static void ProcessPendingFileEvents()
	{
		PROFILE_FUNCTION();

		std::vector<std::pair<std::filesystem::path, FileStatus>> events;
		{
			std::lock_guard<std::mutex> lock(AssetManager::Get().m_PendingEventsMutex);
			events.swap(AssetManager::Get().m_PendingEvents);
		}

		for (const auto& [relativePath, status] : events)
		{
			switch (status)
			{
			case FileStatus::Created:
				break;
			case FileStatus::Modified:
				if (AssetManager::Get().m_Assets.Exists(relativePath))
				{
					ENGINE_DEBUG("Reloading asset {0}", relativePath);
					AssetManager::Get().m_Assets.Get(relativePath)->Reload();
				}
				else if (AssetManager::Get().m_Textures.Exists(relativePath))
				{
					ENGINE_DEBUG("Reloading texture {0}", relativePath);
					AssetManager::Get().m_Textures.Get(relativePath)->Reload();
				}
				break;
			case FileStatus::Erased:
				if (AssetManager::Get().m_Assets.Exists(relativePath))
				{
					ENGINE_ERROR("An asset in use has been deleted! {0}", relativePath);
					AssetManager::Get().m_Assets.Remove(relativePath);
				}
				else if (AssetManager::Get().m_Textures.Exists(relativePath))
				{
					ENGINE_ERROR("A texture in use has been deleted! {0}", relativePath);
					AssetManager::Get().m_Textures.Get(relativePath)->Reload();
				}
				break;
			default:
				break;
			}
		}
	}

	static void Shutdown()
	{
		PROFILE_FUNCTION();
		AssetManager::Get().m_Assets.Clear();
		AssetManager::Get().m_Textures.Clear();
		if(AssetManager::Get().m_VFS)
			AssetManager::Get().m_VFS->Unmount();
		AssetManager::Get().m_VFS.reset();
		AssetManager::Get().m_BundleData.clear();
	}

private:
	AssetManager();
	~AssetManager() = default;
	static AssetManager& Get();

	AssetLibrary m_Assets;
	TextureLibrary2D m_Textures;
	Ref<VirtualFileSystem> m_VFS;
	std::vector<uint8_t> m_BundleData;

	FileWatcher m_FileWatcher;
	std::mutex m_PendingEventsMutex;
	std::vector<std::pair<std::filesystem::path, FileStatus>> m_PendingEvents;

	static AssetManager* s_Instance;
};
