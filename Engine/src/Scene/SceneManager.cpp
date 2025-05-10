#include "stdafx.h"
#include "SceneManager.h"
#include "Events/SceneEvent.h"
#include "Core/Application.h"
#include "AssetManager.h"
#include "Core/Settings.h"
#include "imgui.h"

Scope<Scene> SceneManager::s_CurrentScene;
std::filesystem::path SceneManager::s_NextFilepath;
SceneState SceneManager::s_SceneState = SceneState::Play;
std::filesystem::path SceneManager::s_EditingScene;

Scene* SceneManager::CurrentScene()
{
	return s_CurrentScene.get();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::ChangeScene(std::filesystem::path filepath)
{
	std::filesystem::path finalpath = filepath;
	if (finalpath.is_relative())
	{
		if (!Application::GetOpenDocument().empty())
		{
			if (std::filesystem::exists(Application::GetWorkingDirectory() / filepath))
			{
				finalpath = Application::GetWorkingDirectory() / filepath;
			}
			else if (std::filesystem::exists(Application::GetOpenDocumentDirectory() / filepath))
			{
				finalpath = Application::GetOpenDocumentDirectory() / filepath;
			}
		}
		else if (std::filesystem::exists(Application::GetWorkingDirectory() / filepath))
		{
			finalpath = Application::GetWorkingDirectory() / filepath;
		}
	}

	if (IsSceneLoaded())
	{
		s_NextFilepath = finalpath;
		if (s_CurrentScene->IsUpdating())
		{
			return false;
		}
		else
		{
			return FinalChangeScene();
		}
	}
	s_NextFilepath = finalpath;
	return FinalChangeScene();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::IsSceneLoaded()
{
	return s_CurrentScene == nullptr ? false : !s_CurrentScene->IsSaving();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::Update(float deltaTime)
{
	if ((s_SceneState == SceneState::Play || s_SceneState == SceneState::Simulate)
		&& IsSceneLoaded() && !s_CurrentScene->IsUpdating())
	{
		s_CurrentScene->OnUpdate(deltaTime);
	}

	if (!s_NextFilepath.empty())
	{
		FinalChangeScene();
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::FixedUpdate()
{
	if ((s_SceneState == SceneState::Play || s_SceneState == SceneState::Simulate)
		&& IsSceneLoaded() && !s_CurrentScene->IsUpdating())
	{
		s_CurrentScene->OnFixedUpdate();
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::FinalChangeScene()
{
	if (IsSceneLoaded())
	{
		if (s_CurrentScene->IsDirty())
		{
			s_CurrentScene->Save();
		}

		if (s_SceneState != SceneState::Edit && s_EditingScene.empty())
		{
			s_EditingScene = s_CurrentScene->GetFilepath();
		}

		if (s_SceneState != SceneState::Edit && s_SceneState != SceneState::Pause)
		{
			s_CurrentScene->OnRuntimeStop();
		}
	}

	if (!s_NextFilepath.empty())
		s_CurrentScene = CreateScope<Scene>(s_NextFilepath);

	if (s_NextFilepath.is_relative() && AssetManager::HasBundle()) {
		std::vector<uint8_t> data;
		AssetManager::GetFileData(s_NextFilepath.string(), data);
		if (!s_CurrentScene->Load(data))
			s_CurrentScene.reset();
	}
	else if (!s_CurrentScene->Load())
		s_CurrentScene.reset();

	SceneChangedEvent event(s_NextFilepath);
	Application::CallEvent(event);

	const char* title = Application::GetWindow()->GetTitle();

	s_CurrentScene->OnViewportResize(Settings::GetInt(title, "Window_Width"), Settings::GetInt(title, "Window_Height"));

	s_NextFilepath.clear();

	if ((s_SceneState == SceneState::Play || s_SceneState == SceneState::Simulate) && IsSceneLoaded())
		s_CurrentScene->OnRuntimeStart(false);

	AssetManager::CleanUp();

	return IsSceneLoaded();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::CreateScene(std::filesystem::path filename)
{
	Ref<Scene> newScene = CreateRef<Scene>(filename);

	newScene->Save(false);

	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::ChangeSceneState(SceneState sceneState)
{
	if (sceneState != s_SceneState)
	{
		if (IsSceneLoaded())
		{
			if (s_SceneState != SceneState::Pause && s_SceneState != SceneState::SimulatePause)
			{
				if (sceneState == SceneState::Play || sceneState == SceneState::Simulate)
					s_CurrentScene->OnRuntimeStart();
			}
			if (s_SceneState != SceneState::Edit && sceneState == SceneState::Edit)
				s_CurrentScene->OnRuntimeStop();
			if (sceneState == SceneState::Pause || sceneState == SceneState::SimulatePause)
				s_CurrentScene->OnRuntimePause();
		}
		s_SceneState = sceneState;

		if (!s_EditingScene.empty() && s_SceneState == SceneState::Edit)
		{
			ChangeScene(s_EditingScene);
			s_EditingScene.clear();
		}
		ImGuiIO& io = ImGui::GetIO();
		if (sceneState == SceneState::Play) {
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
		}
		else {
			Application::GetWindow()->EnableCursor();
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		}
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

SceneState SceneManager::GetSceneState()
{
	return s_SceneState;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void SceneManager::Restart()
{
	if (!s_EditingScene.empty())
	{
		ChangeScene(s_EditingScene);
	}

	if (IsSceneLoaded())
	{
		s_CurrentScene->OnRuntimeStop();
		s_CurrentScene->OnRuntimeStart();
	}
}

void SceneManager::Shutdown()
{
	if (s_SceneState != SceneState::Edit && IsSceneLoaded())
		s_CurrentScene->OnRuntimeStop();

	s_CurrentScene.reset();
	s_NextFilepath.clear();
	s_EditingScene.clear();
}
