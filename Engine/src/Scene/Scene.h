#pragma once

#include "EnTT/entt.hpp"

class Entity;

class Scene
{
public:
	explicit Scene(std::string name);
	~Scene();

	Entity CreateEntity(const std::string& name = "");

	void OnUpdate(float deltaTime);
	void OnViewportResize(uint32_t width, uint32_t height);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }

private:
	entt::registry m_Registry;

protected:
	std::string m_SceneName;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	friend class Entity;
};