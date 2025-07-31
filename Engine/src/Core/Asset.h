#pragma once

#include "Core/core.h"
#include "Core/UUID.h"
#include "Core/Factory.h"
#include "Core/VirtualFileSystem.h"
#include "Core/Application.h"
#include "Utilities/FileWatcher.h"
#include "Logging/Instrumentor.h"

#include <filesystem>


class Asset
{
public:
	virtual const std::filesystem::path& GetFilepath() const { return m_Filepath; };

	virtual bool Load(const std::filesystem::path& filepath) = 0;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) = 0;

	virtual bool Reload() { return Load(m_Filepath); }
	const Uuid& GetUUID() const { return m_Uuid; }
protected:
	std::filesystem::path m_Filepath;
	Uuid m_Uuid;
};

class AssetLibrary
{
public:
	AssetLibrary() = default;

	void Add(const Ref<Asset>& resource)
	{
		PROFILE_FUNCTION();
		CORE_ASSERT(!Exists(resource->GetFilepath()), "Asset already exists!");
		if(!resource->GetFilepath().empty())
			m_Assets[resource->GetFilepath()] = resource;
	}

	void Remove(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		auto it = m_Assets.find(filepath);
		if (it != m_Assets.end())
		{
			m_Assets.erase(it);
		}
		else
		{
			ENGINE_ERROR("Asset not found: {0}", filepath);
		}
	}

	template<typename T>
	Ref<T> Load(const std::filesystem::path& filepath, Ref<VirtualFileSystem> vfs)
	{
		PROFILE_FUNCTION();
		if (Exists(filepath))
			return std::dynamic_pointer_cast<T>(m_Assets[filepath].lock());

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
			else {
				ENGINE_ERROR("Asset does not exist in bundle: {0}", filepath);
				return nullptr;
			}
		}
		else {
			ENGINE_ERROR("Asset does not exist: {0}", filepath);
			return nullptr;
		}
		Add(asset);
		return std::dynamic_pointer_cast<T>(asset);
	}

	Ref<Asset> Get(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		CORE_ASSERT(Exists(filepath), "Asset does not Exist!");
		return m_Assets[filepath].lock();
	}

	template<typename T>
	Ref<T> Get(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		CORE_ASSERT(Exists(filepath), "Asset does not Exist!");
		return std::dynamic_pointer_cast<T>(m_Assets[filepath].lock());
	}

	bool Exists(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		if (m_Assets.find(filepath) != m_Assets.end())
			return m_Assets[filepath].use_count() > 0;
		return false;
	}

	void Clear()
	{
		PROFILE_FUNCTION();
		m_Assets.clear();
	}

	void CleanUnused()
	{
		PROFILE_FUNCTION();
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
	std::unordered_map<std::filesystem::path, std::weak_ptr<Asset>> m_Assets;
};