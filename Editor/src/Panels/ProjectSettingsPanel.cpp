#include "ProjectSettingsPanel.h"

#include "IconsFontAwesome6.h"
#include "Engine.h"

#include "cereal/archives/json.hpp"
#include "MainDockSpace.h"
#include "ImGui/ImGuiFileEdit.h"
#include "FileSystem/Directory.h"

ProjectSettingsPanel::ProjectSettingsPanel(bool* show)
	:m_Show(show), Layer("Project Settings Panel")
{
}

void ProjectSettingsPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(ICON_FA_GEARS" Project Settings", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		if (ImGui::BeginCombo("Default Scene", m_DefaultScenePath.filename().string().c_str()))
		{
			for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::SCENE)))
			{
				const bool is_selected = false;
				if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
				{
					m_DefaultScenePath = file;
					break;
				}
				ImGui::Tooltip(file.string().c_str());
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_FOLDER_OPEN"##OpenScene"))
		{
			SceneManager::ChangeScene(m_DefaultScenePath);
		}
		ImGui::Tooltip("Open Scene");

		ImGui::InputTextMultiline("Description", m_DescriptionBuffer, sizeof(m_DescriptionBuffer));
		if (ImGui::Button(ICON_FA_FLOPPY_DISK" Save"))
		{
			SaveProjectFile();
		}
	}
	ImGui::End();
}

void ProjectSettingsPanel::OnAttach()
{
	ReadProjectFile();
}

void ProjectSettingsPanel::OnDetach()
{
	SaveProjectFile();
}

void ProjectSettingsPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<AppOpenDocumentChangedEvent>(BIND_EVENT_FN(ProjectSettingsPanel::OnOpenDocumentChanged));
}

void ProjectSettingsPanel::ReadProjectFile()
{
	std::ifstream file(Application::GetOpenDocument());

	if (!file.is_open()) return;

	cereal::JSONInputArchive input(file);

	input(m_ProjectData);

	file.close();

	m_DefaultScenePath = m_ProjectData.defaultScene;

	memset(m_DescriptionBuffer, 0, sizeof(m_DescriptionBuffer));
	for (int i = 0; i < m_ProjectData.description.length(); i++)
	{
		m_DescriptionBuffer[i] = m_ProjectData.description[i];
	}
}

void ProjectSettingsPanel::SaveProjectFile()
{
	m_ProjectData.defaultScene = m_DefaultScenePath.string();
	m_ProjectData.description = m_DescriptionBuffer;

	const std::filesystem::path& projectFile = Application::GetOpenDocument();
	std::ofstream file(projectFile);
	{
		cereal::JSONOutputArchive output(file);

		output(cereal::make_nvp(projectFile.filename().string(), m_ProjectData));
	}

	file.close();
}
