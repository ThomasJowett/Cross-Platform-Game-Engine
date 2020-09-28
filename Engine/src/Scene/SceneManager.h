#pragma once

#include "Core/core.h"

#include "Scene/Scene.h"

class SceneManager
{
public:
	static Scope<Scene> s_CurrentScene;
};