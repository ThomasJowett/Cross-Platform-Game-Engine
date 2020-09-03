#include "ImGuiProjectsStartScreen.h"

#include "include.h"

#include "IconsFontAwesome5.h"

#include "FileSystem/FileDialog.h"
#include "ImGuiDockSpace.h"

ImGuiProjectsStartScreen::ImGuiProjectsStartScreen()
{
	m_CreateProject = false;
}

void ImGuiProjectsStartScreen::OnImGuiRender()
{
	ImGui::OpenPopup("##StartScreen");

	int windowSizeX = Application::GetWindow().GetWidth();
	int windowSizeY = Application::GetWindow().GetHeight();

	int popupSizeX = std::min(windowSizeX - 20, 600);
	int popupSizeY = std::min(windowSizeY - 20, 600);

	ImGui::SetNextWindowSize(ImVec2(popupSizeX, popupSizeY));

	int windowPosX = Application::GetWindow().GetPosX();
	int windowPosY = Application::GetWindow().GetPosY();

	// Find the middle of the screen
	ImGui::SetNextWindowPos(ImVec2(
		(float)(windowSizeX / 2.0f) + windowPosX - (float)(popupSizeX / 2.0f),
		(float)(windowSizeY / 2.0f) + windowPosY - (float)(popupSizeY / 2.0f))
	);

	ImGuiWindowFlags flags = ImGuiWindowFlags_Modal |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize;

	if (ImGui::BeginPopupModal("##StartScreen", NULL, flags))
	{
		if (!m_CreateProject)
		{
			ImGui::Columns(2);
			ImGui::Text("Recent Projects...");
			ImGui::NextColumn();
			m_CreateProject = ImGui::Button(ICON_FA_FOLDER_PLUS" New Project");
			if (ImGui::Button(ICON_FA_FOLDER_OPEN" Browse Local"))
			{
				Application::SetOpenDocument(FileDialog(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*|0"));
				ImGui::CloseCurrentPopup();
				Application::Get().RemoveOverlay(this);
			}
		}
		else
		{
			ImGui::Text("Select Template...");
			ImGui::Separator();

			if (ImGui::Button("Blank Project"))
			{
				std::filesystem::path projectPath = SaveAsDialog(L"Choose Project Location...", L"Project File (*.proj)\0*.proj");

				if (projectPath.empty())
				{
					ImGui::EndPopup();
					return;
				}

				projectPath.replace_extension(".proj");

				if (std::filesystem::exists(projectPath))
				{
					CLIENT_ERROR("Project already exist, Please choose a different location.");
				}
				else
				{
					std::ofstream file;
					file.open(projectPath);
					if (file.is_open())
					{
						file.close();
						Application::SetOpenDocument(projectPath);
						ImGui::CloseCurrentPopup();
						Application::Get().RemoveOverlay(this);
					}
				}
			}
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