#include "ProjectSettingsPanel.h"

#include "IconsFontAwesome5.h"
#include "Engine.h"

#include "cereal/archives/json.hpp"

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
	if (ImGui::Begin(ICON_FA_COGS" Project Settings", m_Show))
	{
		ImGui::InputText("Default Scene", m_DefaultSceneBuffer, sizeof(m_DefaultSceneBuffer));
		ImGui::InputTextMultiline("Description", m_DescriptionBuffer, sizeof(m_DescriptionBuffer));
		if (ImGui::Button(ICON_FA_SAVE" Save"))
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
	dispatcher.Dispatch<AppOpenDocumentChange>(BIND_EVENT_FN(ProjectSettingsPanel::OnOpenDocumentChanged));
}

void ProjectSettingsPanel::ReadProjectFile()
{
	std::ifstream file(Application::GetOpenDocument());

	if (!file.is_open()) return;

	cereal::JSONInputArchive input(file);

	input(m_ProjectData);

	file.close();

	for (int i = 0; i < m_ProjectData.defaultScene.length(); i++)
	{
		m_DefaultSceneBuffer[i] = m_ProjectData.defaultScene[i];
	}

	for (int i = 0; i < m_ProjectData.description.length(); i++)
	{
		m_DescriptionBuffer[i] = m_ProjectData.description[i];
	}
}

void ProjectSettingsPanel::SaveProjectFile()
{
	m_ProjectData.defaultScene = m_DefaultSceneBuffer;
	m_ProjectData.description = m_DescriptionBuffer;

	const std::filesystem::path& projectFile = Application::GetOpenDocument();
	std::ofstream file(projectFile);
	{
		cereal::JSONOutputArchive output(file);

		output(cereal::make_nvp(projectFile.filename().string(), m_ProjectData));
	}

	file.close();
}
