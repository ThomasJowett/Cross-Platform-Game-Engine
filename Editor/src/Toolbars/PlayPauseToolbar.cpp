#include "PlayPauseToolbar.h"

#include "IconsFontAwesome5.h"

#include "ImGui/ImGuiUtilites.h"

void PlayPauseToolbar::Render()
{

	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
	static bool isPlaying = false;
	if (isPlaying)
	{
		if (ImGui::Button(ICON_FA_PAUSE "##Pause"))
			isPlaying = false;
		ImGui::Tooltip("Pause");
		ImGui::SameLine();

		if(ImGui::Button(ICON_FA_STOP "##Stop"))
			isPlaying = false;
		ImGui::Tooltip("Stop");
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_UNDO_ALT "##Restart"))
			isPlaying = false;
		ImGui::Tooltip("Restart");
		ImGui::SameLine();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY "##Play"))
			isPlaying = true;
		ImGui::Tooltip("Play");
		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PLAY_CIRCLE "##Simulate"))
			isPlaying = true;
		ImGui::Tooltip("Simulate");
	}
	ImGui::PopStyleColor();
}
