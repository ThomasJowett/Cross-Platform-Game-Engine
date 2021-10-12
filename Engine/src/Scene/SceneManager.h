#pragma once

#include "Core/core.h"

#include "Scene/Scene.h"

enum class SceneState
{
	Play = 0,
	Pause = 1,
	Simulate = 2,
	Edit = 3
};

class SceneManager
{
public:
	static Scene* CurrentScene();
	static bool ChangeScene(std::filesystem::path filepath);
	static bool ChangeScene(std::string name);
	static bool IsSceneLoaded();
	static bool Update(float deltaTime);
	static bool FixedUpdate();
	static bool CreateScene(std::filesystem::path filename);
	static bool ChangeSceneState(SceneState sceneState);
private:
	static bool FinalChangeScene();
	static Scope<Scene> s_CurrentScene;
	static std::filesystem::path s_NextFilepath;
	static std::string s_NextSceneName;
	static SceneState s_SceneState;
};