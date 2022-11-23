#pragma once

#include "Core/core.h"

#include "Scene/Scene.h"

enum class SceneState
{
	Play = 0,
	Pause = 1,
	Simulate = 2,
	SimulatePause = 3,
	Edit = 4
};

class SceneManager
{
public:
	static Scene* CurrentScene();
	static bool ChangeScene(std::filesystem::path filepath);
	static bool IsSceneLoaded();
	static bool Update(float deltaTime);
	static bool FixedUpdate();
	static bool CreateScene(std::filesystem::path filename);
	static bool ChangeSceneState(SceneState sceneState);
	static SceneState GetSceneState();
	static void Restart();
	static void Shutdown();
private:
	static bool FinalChangeScene();
	static Scope<Scene> s_CurrentScene;
	static std::filesystem::path s_NextFilepath;
	static SceneState s_SceneState;
	static std::filesystem::path s_EditingScene;
	static bool s_BinaryScene;
};