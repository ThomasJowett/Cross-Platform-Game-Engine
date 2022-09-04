#pragma once

#include "Core/core.h"

#include <filesystem>

class Asset
{
public:
	virtual bool Load(const std::filesystem::path& filepath) = 0;

	bool Reload() { return Load(m_Filepath); }

protected:
	std::filesystem::path m_Filepath;
};

template<typename T>
class AssetLibrary
{
public:
	void Add(const Ref<T>& resource);
	Ref<T> Load(const std::filesystem::path& filepath);
	Ref<T> Get(const std::string& name);
	bool Exists(const std::string& name);
	void Clear();

private:
	std::unordered_map<std::string, std::weak_ptr<T>> m_Assets;
};

template<typename T>
inline void AssetLibrary<T>::Add(const Ref<T>& resource)
{
	CORE_ASSERT(!Exists(resource->GetFilepath().filename().string()), "Asset already exists!");
	m_Assets[resource->GetFilepath().filename().string()] = resource;
}

template<typename T>
inline Ref<T> AssetLibrary<T>::Load(const std::filesystem::path& filepath)
{
	if (Exists(filepath.filename().string()))
		return m_Assets[filepath.filename().string()].lock();

	Ref<T> asset = CreateRef<T>(filepath);
	Add(asset);
	return asset;
}

template<typename T>
inline Ref<T> AssetLibrary<T>::Get(const std::string& name)
{
	CORE_ASSERT(Exists(name), "Asset does not Exist!");
	return m_Assets[name].lock();
}

template<typename T>
inline bool AssetLibrary<T>::Exists(const std::string& name)
{
	if (m_Assets.find(name) != m_Assets.end())
		return m_Assets[name].use_count() > 0;
	return false;
}

template<typename T>
inline void AssetLibrary<T>::Clear()
{
	m_Assets.clear();
}
