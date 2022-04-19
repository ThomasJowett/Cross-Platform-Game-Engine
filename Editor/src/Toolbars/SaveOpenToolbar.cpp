#include "SaveOpenToolbar.h"

#include "IconsFontAwesome5.h"
#include "Viewers/ViewerManager.h"

#include "ImGui/ImGuiUtilites.h"
#include "Scene/SceneManager.h"

void SaveOpenToolbar::Render()
{
	bool sceneLoaded = SceneManager::IsSceneLoaded();
	ImGui::BeginDisabled(!sceneLoaded);

	if (ImGui::Button(ICON_FA_SAVE "## Save"))
	{
		SceneManager::CurrentScene()->Save();

		ViewerManager::SaveAll();
	}
	ImGui::Tooltip("Save all");

	ImGui::SameLine();

	ImGui::EndDisabled();
}
