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
		ReadCollisionLayersFile();
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

		DrawCollisionLayers();

		if (ImGui::Button(ICON_FA_FLOPPY_DISK" Save"))
		{
			SaveProjectFile();
			SaveCollisionLayersFile();
		}
	}
	ImGui::End();
}

void ProjectSettingsPanel::OnAttach()
{
	ReadProjectFile();
	ReadCollisionLayersFile();
}

void ProjectSettingsPanel::OnDetach()
{
	SaveProjectFile();
	SaveCollisionLayersFile();
}

void ProjectSettingsPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<AppOpenDocumentChangedEvent>(BIND_EVENT_FN(ProjectSettingsPanel::OnOpenDocumentChanged));
}

void ProjectSettingsPanel::ReadProjectFile()
{
	// No project open yet: nothing to read, so don't report a missing file
	if (Application::GetOpenDocument().empty()
		|| !ProjectSerializer::Deserialize(m_ProjectData, Application::GetOpenDocument()))
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
		if (!Application::GetOpenDocument().empty())
			ENGINE_ERROR("Not saving project settings: the current project file couldn't be read");
		return;
	}

	m_ProjectData.defaultScene = m_DefaultScenePath.string();
	m_ProjectData.description = m_DescriptionBuffer;

	ProjectSerializer::Serialize(m_ProjectData, Application::GetOpenDocument());
}

void ProjectSettingsPanel::ReadCollisionLayersFile()
{
	m_CollisionLayers = CollisionLayers::DefaultNames();
	m_CollisionLayersDirty = false;

	if (Application::GetOpenDocumentDirectory().empty())
		return;

	CollisionLayers::Load(m_CollisionLayers, Application::GetOpenDocumentDirectory() / CollisionLayers::FilePath);
}

void ProjectSettingsPanel::SaveCollisionLayersFile()
{
	if (!m_CollisionLayersDirty || Application::GetOpenDocumentDirectory().empty())
		return;

	std::filesystem::path filepath = Application::GetOpenDocumentDirectory() / CollisionLayers::FilePath;

	std::error_code errorCode;
	std::filesystem::create_directories(filepath.parent_path(), errorCode);
	if (errorCode)
	{
		ENGINE_ERROR("Could not create directory for collision layers file: {0}, {1}", filepath.parent_path().string(), errorCode.message());
		return;
	}

	if (!CollisionLayers::Save(m_CollisionLayers, filepath))
	{
		ENGINE_ERROR("Could not save collision layers file: {0}", filepath.string());
		return;
	}

	CollisionLayers::SetNames(m_CollisionLayers);
	m_CollisionLayersDirty = false;
}

void ProjectSettingsPanel::DrawCollisionLayers()
{
	if (!ImGui::CollapsingHeader("Collision Layers"))
		return;

	ImGui::TextDisabled("Name a slot to add a layer, clear it to remove the layer");

	if (ImGui::BeginTable("##CollisionLayers", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp))
	{
		ImGui::TableSetupColumn("##Index", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch);

		for (int i = 0; i < CollisionLayers::MaxLayers; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("%d", i);

			ImGui::TableNextColumn();
			char nameBuffer[64];
			strncpy(nameBuffer, m_CollisionLayers[i].c_str(), sizeof(nameBuffer) - 1);
			nameBuffer[sizeof(nameBuffer) - 1] = '\0';

			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::BeginDisabled(i == 0);
			if (ImGui::InputTextWithHint("##LayerName", "Unused", nameBuffer, sizeof(nameBuffer)))
			{
				m_CollisionLayers[i] = nameBuffer;
				m_CollisionLayersDirty = true;
			}
			ImGui::EndDisabled();

			if (!m_CollisionLayers[i].empty())
			{
				for (int j = 0; j < i; ++j)
				{
					if (m_CollisionLayers[j] == m_CollisionLayers[i])
					{
						Colour textColour(Colours::YELLOW);
						ImGui::TextColored(ImVec4(textColour.r, textColour.g, textColour.b, textColour.a), "Duplicate name, Lua lookups find layer %d", j);
						break;
					}
				}
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
}
