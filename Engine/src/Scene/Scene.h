#pragma once

#include "EnTT/entt.hpp"

class Entity;

class Scene
{
public:
	explicit Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = "");

	void OnUpdate(float deltaTime);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }
private:
	entt::registry m_Registry;
protected:
	std::string m_SceneName;

	friend class Entity;
};

