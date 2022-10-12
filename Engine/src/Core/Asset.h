#pragma once

#include "Core/core.h"

#include <filesystem>

class Asset
{
public:
	virtual bool Load(const std::filesystem::path& filepath) = 0;
	virtual const std::filesystem::path& GetFilepath() const = 0;

	bool Reload() { return Load(m_Filepath); }

protected:
	std::filesystem::path m_Filepath;
};

template<typename T>
class AssetLibrary
{
public:
	AssetLibrary<T>() {}
	void Add(const Ref<T>& resource);
	Ref<T> Load(const std::filesystem::path& filepath);
	Ref<T> Get(const std::filesystem::path& filepath);
	bool Exists(const std::filesystem::path& filepath);
	void Clear();

private:
	std::unordered_map<std::string, std::weak_ptr<T>> m_Assets;
};

template<typename T>
inline void AssetLibrary<T>::Add(const Ref<T>& resource)
{
	CORE_ASSERT(!Exists(resource->GetFilepath()), "Asset already exists!");
	m_Assets[resource->GetFilepath().string()] = resource;
}

template<typename T>
inline Ref<T> AssetLibrary<T>::Load(const std::filesystem::path& filepath)
{
	if (Exists(filepath))
		return m_Assets[filepath.string()].lock();

	Ref<T> asset = CreateRef<T>(filepath);
	Add(asset);
	return asset;
}

template<typename T>
inline Ref<T> AssetLibrary<T>::Get(const std::filesystem::path& filepath)
{
	CORE_ASSERT(Exists(filepath), "Asset does not Exist!");
	return m_Assets[filepath.string()].lock();
}

template<typename T>
inline bool AssetLibrary<T>::Exists(const std::filesystem::path& filepath)
{
	if (m_Assets.find(filepath.string()) != m_Assets.end())
		return m_Assets[filepath.string()].use_count() > 0;
	return false;
}

template<typename T>
inline void AssetLibrary<T>::Clear()
{
	m_Assets.clear();
}
