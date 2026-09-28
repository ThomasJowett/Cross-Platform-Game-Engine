#include "ProjectSettingsPanel.h"

#include "IconsFontAwesome6.h"

#include "ProjectSerializer.h"
#include "MainDockSpace.h"
#include "ImGui/ImGuiFileEdit.h"
#include "FileSystem/Directory.h"
#include "ImGui/ImGuiUtilities.h"
#include "Scene/SceneManager.h"
#include "Viewers/ViewerManager.h"
#include "Utilities/FileUtils.h"

ProjectSettingsPanel::ProjectSettingsPanel(bool* show)
	:m_Show(show), Layer("Project Settings Panel")
{
}

void ProjectSettingsPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		m_WasShown = false;
		return;
	}

	// The .proj file can change on disk without an AppOpenDocumentChangedEvent firing (e.g. a
	// scene rename updating the Default Scene field) - re-read on every hidden-to-shown
	// transition so reopening this window reflects that instead of showing stale cached data.
	if (!m_WasShown)
	{
		ReadProjectFile();
		m_WasShown = true;
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
					// Directory::GetFilesRecursive returns absolute paths - store project-relative,
					// matching what's actually written to the project file.
					m_DefaultScenePath = FileUtils::RelativePath(file, Application::GetOpenDocumentDirectory());
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

		int pageSize = (int)m_ProjectData.spriteAtlasPageSize;
		if (ImGui::InputInt("Sprite Atlas Page Size", &pageSize, 256, 1024))
			m_ProjectData.spriteAtlasPageSize = (uint32_t)std::clamp(pageSize, 256, 8192);
		ImGui::Tooltip("Dimensions (square, pixels) of each packed sprite atlas page. Larger pages\nmean fewer pages but more VRAM per page - changing this only takes effect\non the next atlas rebuild.");

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
	if (!ProjectSerializer::Deserialize(m_ProjectData, Application::GetOpenDocument()))
	{
		// Don't leave whatever was read for the previous project sitting in memory - otherwise
		// a project whose file fails to parse (e.g. still the old cereal-JSON .proj format)
		// looks like it "has" the last successfully-opened project's settings, and hitting
		// Save would write that other project's data into this one's file.
		m_ProjectData = ProjectData();
		m_DefaultScenePath.clear();
		memset(m_DescriptionBuffer, 0, sizeof(m_DescriptionBuffer));
		m_Loaded = false;
		return;
	}

	m_DefaultScenePath = m_ProjectData.defaultScene;

	memset(m_DescriptionBuffer, 0, sizeof(m_DescriptionBuffer));
	for (int i = 0; i < m_ProjectData.description.length(); i++)
	{
		m_DescriptionBuffer[i] = m_ProjectData.description[i];
	}
	m_Loaded = true;
}

void ProjectSettingsPanel::SaveProjectFile()
{
	// Never successfully read a project file for the currently open project (see
	// ReadProjectFile) - refuse to save rather than overwriting it with empty/stale data.
	if (!m_Loaded)
	{
		ENGINE_ERROR("Not saving project settings: the current project file couldn't be read");
		return;
	}

	m_ProjectData.defaultScene = m_DefaultScenePath.string();
	m_ProjectData.description = m_DescriptionBuffer;

	ProjectSerializer::Serialize(m_ProjectData, Application::GetOpenDocument());
}
