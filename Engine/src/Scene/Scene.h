#pragma once

#include "EnTT/entt.hpp"

class Scene
{
public:
	Scene();
	~Scene();
private:
	entt::registry m_Registry;
};

