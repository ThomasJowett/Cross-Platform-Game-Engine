#pragma once

#include "Core/core.h"
#include "Core/UUID.h"
#include "Core/Factory.h"
#include "Core/VirtualFileSystem.h"
#include "Core/Application.h"
#include "Utilities/FileWatcher.h"

#include <filesystem>


class Asset
{
public:
	virtual const std::filesystem::path& GetFilepath() const { return m_Filepath; };

	virtual bool Load(const std::filesystem::path& filepath) = 0;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) = 0;

	bool Reload() { return Load(m_Filepath); }
	const Uuid& GetUUID() const { return m_Uuid; }
protected:
	std::filesystem::path m_Filepath;
	Uuid m_Uuid;
};

class AssetLibrary
{
public:
	AssetLibrary(const std::filesystem::path& directory)
		:m_FileWatcher(std::chrono::milliseconds(2000))
	{
		m_FileWatcher.SetPathToWatch(directory);
		m_FileWatcher.Start([=](std::string path, FileStatus status)
			{
				switch (status)
				{
				case FileStatus::Created:
					break;
				case FileStatus::Modified:
					if (Exists(path))
					{
						ENGINE_DEBUG("Reloading asset {0}", path);
						m_Assets[path].lock()->Reload();
					}
					break;
				case FileStatus::Erased:
					if (Exists(path))
					{
						ENGINE_ERROR("An asset in use has been deleted! {0}", path);
						m_Assets.at(path).reset();
						m_Assets.erase(path);
					}
					break;
				default:
					break;
				}
			});
	}

	void Add(const Ref<Asset>& resource)
	{
		CORE_ASSERT(!Exists(resource->GetFilepath()), "Asset already exists!");
		if(!resource->GetFilepath().empty())
			m_Assets[resource->GetFilepath().string()] = resource;
	}

	template<typename T>
	Ref<T> Load(const std::filesystem::path& filepath, Ref<VirtualFileSystem> vfs)
	{
		if (Exists(filepath))
			return std::dynamic_pointer_cast<T>(m_Assets[filepath.string()].lock());

		Ref<T> asset;
		std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
		if (std::filesystem::exists(absolutePath)) {
			ENGINE_INFO("Loading asset from disk: {0}", absolutePath);
			asset = CreateRef<T>(filepath);
		}
		else if (vfs)
		{
			if (vfs->Exists(filepath)) {
				std::vector<uint8_t> data;
				if (vfs->ReadFile(filepath, data))
				{
					ENGINE_INFO("Loading asset from bundle: {0}", filepath);
					asset = CreateRef<T>(filepath, data);
				}
				else
				{
					ENGINE_ERROR("Could not load asset from bundle: {0}", filepath);
					return nullptr;
				}
			}
		}
		else {
			ENGINE_ERROR("Asset does not exist: {0}", filepath);
			return nullptr;
		}
		Add(asset);
		return std::dynamic_pointer_cast<T>(asset);
	}

	template<typename T>
	Ref<T> Get(const std::filesystem::path& filepath)
	{
		CORE_ASSERT(Exists(filepath), "Asset does not Exist!");
		return std::dynamic_pointer_cast<T>(m_Assets[filepath.string()].lock());
	}

	bool Exists(const std::filesystem::path& filepath)
	{
		if (m_Assets.find(filepath.string()) != m_Assets.end())
			return m_Assets[filepath.string()].use_count() > 0;
		return false;
	}

	void Clear()
	{
		m_Assets.clear();
	}

	void CleanUnused()
	{
		auto iter = m_Assets.begin();
		while (iter != m_Assets.end())
		{
			if (iter->second.use_count() <= 0)
				iter = m_Assets.erase(iter);
			else
				++iter;
		}
	}

private:
	std::unordered_map<std::string, std::weak_ptr<Asset>> m_Assets;
	FileWatcher m_FileWatcher;
};


