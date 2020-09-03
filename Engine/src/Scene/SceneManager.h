#pragma once

#include "Core/core.h"

class Scene;


class SceneManager
{
public:
	static Ref<Scene> GetCurrentScene() { return s_CurrentScene; }
	static void SetScene(Ref<Scene> scene) { s_CurrentScene = scene; }
private:

	static Ref<Scene> s_CurrentScene;
};