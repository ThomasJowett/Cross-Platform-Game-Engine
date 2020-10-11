#pragma once

#include <filesystem>

#include "EnTT/entt.hpp"

class Entity;

class Scene
{
public:
	explicit Scene(std::filesystem::path filepath);
	~Scene();

	Entity CreateEntity(const std::string& name = "");

	bool RemoveEntity(const Entity& entity);

	void OnUpdate(float deltaTime);
	void OnViewportResize(uint32_t width, uint32_t height);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }

	virtual void Save(bool binary = false);
	virtual void Save(std::filesystem::path filepath, bool binary = false);
	virtual void Load(bool binary = false);

	void MakeDirty() { m_Dirty = true; }
	bool IsDirty() { return m_Dirty; }

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