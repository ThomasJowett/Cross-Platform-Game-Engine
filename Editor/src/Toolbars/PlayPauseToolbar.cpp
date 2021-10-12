#include "PlayPauseToolbar.h"

#include "IconsFontAwesome5.h"

#include "ImGui/ImGuiUtilites.h"
#include "Scene/SceneManager.h"

void PlayPauseToolbar::Render()
{

	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
	static bool isPlaying = false;
	if (isPlaying)
	{
		if (ImGui::Button(ICON_FA_PAUSE "##Pause"))
		{
			isPlaying = false;
			SceneManager::ChangeSceneState(SceneState::Pause);
		}
		ImGui::Tooltip("Pause");
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_STOP "##Stop"))
		{
			isPlaying = false;
			SceneManager::ChangeSceneState(SceneState::Edit);
		}
		ImGui::Tooltip("Stop");
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_UNDO_ALT "##Restart"))
		{
			isPlaying = false;

		}
		ImGui::Tooltip("Restart");
		ImGui::SameLine();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY "##Play"))
		{
			isPlaying = true;
			SceneManager::ChangeSceneState(SceneState::Play);
		}
		ImGui::Tooltip("Play");
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PLAY_CIRCLE "##Simulate"))
		{
			isPlaying = true;
			SceneManager::ChangeSceneState(SceneState::Simulate);
		}
		ImGui::Tooltip("Simulate");
	}
	ImGui::PopStyleColor();
}
