#include "ProjectsStartScreen.h"

#include "Engine.h"

#include "IconsFontAwesome5.h"

#include "FileSystem/FileDialog.h"
#include "MainDockSpace.h"

#include "ProjectData.h"
#include "cereal/archives/json.hpp"

ProjectsStartScreen::ProjectsStartScreen()
{
	m_CreateProject = false;
}

void ProjectsStartScreen::OnImGuiRender()
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

			for (const std::filesystem::path& project : m_RecentProjects)
			{
				if (ImGui::Button(project.filename().string().c_str()))
				{
					OpenProject(project);
					break;
				}
			}

			ImGui::NextColumn();
			m_CreateProject = ImGui::Button(ICON_FA_FOLDER_PLUS" New Project");
			if (ImGui::Button(ICON_FA_FOLDER_OPEN" Browse Local"))
			{
				OpenProject(FileDialog(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*|0"));
			}
		}
		else
		{
			ImGui::Text("Select Template...");
			ImGui::SameLine(0.0f, popupSizeX - 185);
			if (ImGui::Button(ICON_FA_LONG_ARROW_ALT_LEFT" Back"))
			{
				m_CreateProject = false;
			}
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
						std::filesystem::path sceneDirectory = projectPath;
						sceneDirectory.remove_filename();
						sceneDirectory /= "Scenes";

						std::filesystem::create_directory(sceneDirectory);

						std::string sceneName = "Untitled.scene";

						ProjectData data;
						data.DefaultScene = "Scenes/" + sceneName;

						Ref<Scene> newScene = CreateRef<Scene>(sceneDirectory / sceneName);

						newScene->Save(false);

						{
							cereal::JSONOutputArchive output(file);
							output(cereal::make_nvp(projectPath.string(), data));
						}

						file.close();
						OpenProject(projectPath);
					}
				}
			}
		}
		ImGui::EndPopup();
	}
}

void ProjectsStartScreen::OnAttach()
{
	std::string recentProjectsStr = Settings::GetValue("Files", "Recent_Files");

	std::vector<std::string> recentProjectsList = SplitString(recentProjectsStr, ',');

	for (std::filesystem::path project : recentProjectsList)
	{
		if (project.extension() == ".proj")
		{
			if (std::filesystem::exists(project))
				m_RecentProjects.push_back(project);
		}
	}
}

void ProjectsStartScreen::OnDetach()
{
}

void ProjectsStartScreen::OpenProject(const std::filesystem::path& projectPath)
{
	Application::SetOpenDocument(projectPath);
	ImGui::CloseCurrentPopup();
	Application::Get().RemoveOverlay(this);
}
