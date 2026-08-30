#include "ProjectsStartScreen.h"

#include "Core/Application.h"
#include "Core/Settings.h"
#include "Scene/Scene.h"
#include "Utilities/StringUtils.h"

#include "IconsFontAwesome6.h"

#include "FileSystem/FileDialog.h"

#include "ProjectData.h"
#include "cereal/archives/json.hpp"
#include "imgui.h"

ProjectsStartScreen::ProjectsStartScreen(bool createProject)
	:m_CreateProject(createProject), Layer("Project Start Screen")
{
	m_CanGoBack = !createProject;
}

void ProjectsStartScreen::OnImGuiRender()
{
	ImGui::OpenPopup("##StartScreen");

	int windowSizeX = Application::GetWindow()->GetWidth();
	int windowSizeY = Application::GetWindow()->GetHeight();

	float popupSizeX = (float)std::min(windowSizeX - 20, 600);
	float popupSizeY = (float)std::min(windowSizeY - 20, 600);

	ImGui::SetNextWindowSize(ImVec2(popupSizeX, popupSizeY));

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	// Find the middle of the screen
	ImGui::SetNextWindowPos(ImVec2(
		viewport->Pos.x + (viewport->Size.x * 0.5f) - (float)(popupSizeX * 0.5f),
		viewport->Pos.y + (viewport->Size.y * 0.5f) - (float)(popupSizeY * 0.5f))
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
			ImGui::TextUnformatted("Recent Projects...");

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
				std::optional<std::wstring> fileToOpen = FileDialog::Open(L"Open Project...", { {L"Project Files (*.proj)", L"*.proj"}, {L"Any File", L"*.*"} });
				if (fileToOpen.has_value())
					OpenProject(fileToOpen.value());
			}
		}
		else
		{
			ImGui::TextUnformatted("Select Template...");
			if (m_CanGoBack)
			{
				ImGui::SameLine(0.0f, (float)(popupSizeX - 185));
				if (ImGui::Button(ICON_FA_ARROW_LEFT_LONG" Back"))
				{
					m_CreateProject = false;
				}
			}
			else
			{
				ImGui::SameLine(0.0f, (float)(popupSizeX - 190));
				if (ImGui::Button(ICON_FA_XMARK" Cancel"))
				{
					ImGui::CloseCurrentPopup();
					Application::GetLayerStack().RemoveOverlay(shared_from_this());
					ImGui::EndPopup();
					return;
				}
			}
			ImGui::Separator();

			if (ImGui::Button("Blank Project"))
			{
				std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Choose Project Location...", { {L"Project File (*.proj)", L"*.proj"} });

				if (!dialogPath)
				{
					ImGui::EndPopup();
					return;
				}

				std::filesystem::path projectPath = dialogPath.value();

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
						data.defaultScene = "Scenes/" + sceneName;

						Ref<Scene> newScene = CreateRef<Scene>(sceneDirectory / sceneName);

						newScene->Save(false);

						{
							cereal::JSONOutputArchive output(file);
							output(cereal::make_nvp(projectPath.filename().string(), data));
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
	if(Application::SetOpenDocument(projectPath)){
		ImGui::CloseCurrentPopup();
		Application::GetLayerStack().RemoveOverlay(shared_from_this());
	}
}
