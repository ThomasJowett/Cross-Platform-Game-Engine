#include "PlayPauseToolbar.h"

#include "IconsFontAwesome5.h"

#include "ImGui/ImGuiUtilites.h"
#include "Scene/SceneManager.h"

void PlayPauseToolbar::Render()
{
	static bool isPlaying = false;
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);

	switch (SceneManager::GetSceneState())
	{
	case SceneState::Edit:
	{
		PlayButton();
		SimulateButton();
		break;
	}
	[[fallthrough]];
	case SceneState::Pause:
	case SceneState::SimulatePause:
	{
		PlayButton();
		StopButton();
		RestartButton();
		break;
	}
	[[fallthrough]];
	case SceneState::Play:
	case SceneState::Simulate:
	{
		PauseButton();
		StopButton();
		RestartButton();
		break;
	}
	default:
		break;
	}
	ImGui::PopStyleColor();
}

void PlayPauseToolbar::PlayButton()
{
	if (ImGui::Button(ICON_FA_PLAY "##Play"))
	{
		if (SceneManager::GetSceneState() == SceneState::SimulatePause)
			SceneManager::ChangeSceneState(SceneState::Simulate);
		else if (SceneManager::GetSceneState() == SceneState::Pause
			|| SceneManager::GetSceneState() == SceneState::Edit)
			SceneManager::ChangeSceneState(SceneState::Play);
	}
	ImGui::Tooltip("Play");
	ImGui::SameLine();
}

void PlayPauseToolbar::SimulateButton()
{
	if (ImGui::Button(ICON_FA_PLAY_CIRCLE "##Simulate"))
	{
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
	if (ImGui::Button(ICON_FA_UNDO_ALT "##Restart"))
	{
		SceneManager::Restart();
	}
	ImGui::Tooltip("Restart");
	ImGui::SameLine();
}
