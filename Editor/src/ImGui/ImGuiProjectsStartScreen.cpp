#include "ImGuiProjectsStartScreen.h"

#include "include.h"

#include "IconsFontAwesome5.h"

#include "FileSystem/FileDialog.h"
#include "ImGuiDockSpace.h"

ImGuiProjectsStartScreen::ImGuiProjectsStartScreen()
{
}

void ImGuiProjectsStartScreen::OnImGuiRender()
{
	ImGui::OpenPopup("##StartScreen");

	ImGui::SetNextWindowSize(ImVec2(800, 800));

	ImGuiWindowFlags flags = ImGuiWindowFlags_Modal |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar;

	if (ImGui::BeginPopupModal("##StartScreen", NULL, flags))
	{
		ImGui::Columns(2);
		ImGui::Text("Recent Projects...");
		ImGui::NextColumn();
		ImGui::Button(ICON_FA_FOLDER_PLUS" New Project");
		if (ImGui::Button(ICON_FA_FOLDER_OPEN" Browse Local"))
		{
			Application::SetOpenDocument(FileDialog(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*|0"));
			ImGui::CloseCurrentPopup();
			Application::Get().RemoveOverlay(this);
		}
		ImGui::EndPopup();
	}
}

void ImGuiProjectsStartScreen::OnAttach()
{
}

void ImGuiProjectsStartScreen::OnDetach()
{
}