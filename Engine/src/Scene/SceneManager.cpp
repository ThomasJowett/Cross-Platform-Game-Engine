#include "stdafx.h"
#include "SceneManager.h"
#include "Events/SceneEvent.h"
#include "Core/Application.h"

Scope<Scene> SceneManager::s_CurrentScene;
std::filesystem::path SceneManager::s_NextFilepath;

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
	}

	if (IsSceneLoaded())
	{
		if (s_CurrentScene->GetFilepath() != finalpath)
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
		else
		{
			return false;
		}
	}

	s_CurrentScene = CreateScope<Scene>(finalpath);
	if (!s_CurrentScene->Load(false))
		s_CurrentScene = nullptr;

	return IsSceneLoaded();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::IsSceneLoaded()
{
	return s_CurrentScene == nullptr ? false : !s_CurrentScene->IsSaving();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::Update(float deltaTime)
{
	if (IsSceneLoaded())
	{
		if (!s_CurrentScene->IsUpdating())
		{
			s_CurrentScene->OnUpdate(deltaTime);
		}
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
	if (IsSceneLoaded())
	{
		if (!s_CurrentScene->IsUpdating())
		{
			s_CurrentScene->OnFixedUpdate();
		}
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
	}

	s_CurrentScene = CreateScope<Scene>(s_NextFilepath);
	if (!s_CurrentScene->Load(false))
		s_CurrentScene = nullptr;

	SceneChanged event(s_NextFilepath);
	Application::CallEvent(event);

	s_NextFilepath.clear();

	return IsSceneLoaded();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool SceneManager::CreateScene(std::filesystem::path filename)
{
	Ref<Scene> newScene = CreateRef<Scene>(filename);

	newScene->Save(false);

	return false;
}