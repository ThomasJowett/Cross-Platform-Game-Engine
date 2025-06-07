#include "PlayPauseToolbar.h"

#include "IconsFontAwesome6.h"
#include "Viewers/ViewerManager.h"

#include "ImGui/ImGuiUtilities.h"
#include "Scene/SceneManager.h"

void PlayPauseToolbar::Render()
{
	bool sceneLoaded = SceneManager::IsSceneLoaded();
	ImGui::BeginDisabled(!sceneLoaded);

	if (!sceneLoaded)
		SceneManager::ChangeSceneState(SceneState::Edit);

	switch (SceneManager::GetSceneState())
	{
	case SceneState::Edit:
		PlayButton();
		SimulateButton();
		break;
	case SceneState::Pause:
	case SceneState::SimulatePause:
		PlayButton();
		StopButton();
		RestartButton();
		break;
	case SceneState::Play:
	case SceneState::Simulate:
		PauseButton();
		StopButton();
		RestartButton();
		break;
	default:
		break;
	}

	ImGui::EndDisabled();
}

void PlayPauseToolbar::PlayButton()
{
	if (ImGui::Button(ICON_FA_PLAY "##Play"))
	{
		if (SceneManager::GetSceneState() == SceneState::Edit)
			ViewerManager::SaveAll();

		if (SceneManager::GetSceneState() == SceneState::SimulatePause)
			SceneManager::ChangeSceneState(SceneState::Simulate);
		else if (SceneManager::GetSceneState() == SceneState::Pause
			|| SceneManager::GetSceneState() == SceneState::Edit)
			SceneManager::ChangeSceneState(SceneState::Play);
	}
	ImGui::Tooltip("Play (Alt + P)");
	ImGui::SameLine();
}

void PlayPauseToolbar::SimulateButton()
{
	if (ImGui::Button(ICON_FA_CIRCLE_PLAY "##Simulate"))
	{
		if (SceneManager::GetSceneState() == SceneState::Edit)
			ViewerManager::SaveAll();

		SceneManager::ChangeSceneState(SceneState::Simulate);
	}
	ImGui::Tooltip("Simulation");
}

void PlayPauseToolbar::PauseButton()
{
	if (ImGui::Button(ICON_FA_PAUSE "##Pause"))
	{
		if (SceneManager::GetSceneState() == SceneState::Simulate)
			SceneManager::ChangeSceneState(SceneState::SimulatePause);
		else if (SceneManager::GetSceneState() == SceneState::Play)
			SceneManager::ChangeSceneState(SceneState::Pause);
	}
	ImGui::Tooltip("Pause");
	ImGui::SameLine();
}

void PlayPauseToolbar::StopButton()
{
	if (ImGui::Button(ICON_FA_STOP "##Stop"))
	{
		SceneManager::ChangeSceneState(SceneState::Edit);
	}
	ImGui::Tooltip("Stop");
	ImGui::SameLine();
}

void PlayPauseToolbar::RestartButton()
{
	if (ImGui::Button(ICON_FA_ARROWS_ROTATE "##Restart"))
	{
		SceneManager::Restart();
	}
	ImGui::Tooltip("Restart");
	ImGui::SameLine();
}
