#pragma once

#include "EnTT/entt.hpp"
#include "cereal/access.hpp"

class Entity;

class Scene
{
public:
	explicit Scene(std::filesystem::path filepath);
	~Scene();

	Entity CreateEntity(const std::string& name = "");

	void OnUpdate(float deltaTime);
	void OnViewportResize(uint32_t width, uint32_t height);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }

	virtual void Serialise(bool binary = false);
	virtual void Serialise(std::filesystem::path filepath, bool binary = false);
	virtual void Deserialise(bool binary = false);

	void MakeDirty() { m_Dirty = true; }
	bool IsDirty() { return m_Dirty; }

private:
	friend class cereal::access;

	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("Scene Name", m_SceneName));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("Scene Name", m_SceneName));
	}

private:
	entt::registry m_Registry;

protected:
	std::string m_SceneName;
	std::filesystem::path m_Filepath;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	bool m_Dirty = false;

	friend class Entity;
};