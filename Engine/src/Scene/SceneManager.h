#pragma once

#include "Core/core.h"

#include "Scene/Scene.h"

class SceneManager
{
public:
	static Scene* CurrentScene();
	static bool ChangeScene(std::filesystem::path filepath);
	static bool IsSceneLoaded();
	static bool Update(float deltaTime);
	static bool FixedUpdate();
private:
	static bool FinalChangeScene();
	static Scope<Scene> s_CurrentScene;
	static std::filesystem::path s_NextFilepath;
};