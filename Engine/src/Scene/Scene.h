#pragma once

#include "EnTT/entt.hpp"

class Scene
{
public:
	explicit Scene();
	~Scene();

	entt::entity CreateEntity();

	void OnUpdate(float deltaTime);

	entt::registry& GetRegistry() { return m_Registry; }
	const std::string& GetSceneName() const { return m_SceneName; }
private:
	entt::registry m_Registry;
protected:
	std::string m_SceneName;
};

