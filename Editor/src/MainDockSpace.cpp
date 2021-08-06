#include "MainDockSpace.h"

#ifdef __WINDOWS__
#include <shellapi.h>
#endif // __WINDOWS__

#include "imgui/imgui.h"
#include "Core/Version.h"
#include "Fonts/Fonts.h"
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5Brands.h"
#include "FileSystem/FileDialog.h"
#include "FileSystem/Webpage.h"

#include "ProjectsStartScreen.h"

#include "Panels/ContentExplorerPanel.h"
#include "Panels/JoystickInfoPanel.h"
#include "Panels/ContentExplorerPanel.h"
#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ProjectSettingsPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/ConsolePanel.h"
#include "Toolbars/PlayPauseToolbar.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

#include "Scene/SceneManager.h"

#include "ProjectData.h"
#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"

Layer* MainDockSpace::s_CurrentlyFocusedPanel;

MainDockSpace::MainDockSpace()
	:Layer("Dockspace")
{
	m_Show = true;

	m_ShowEditorPreferences = false;
	m_ShowProjectSettings = false;
	m_ShowViewport = true;
	m_ShowConsole = true;
	m_ShowErrorList = false;
	m_ShowTaskList = false;
	m_ShowProperties = true;
	m_ShowHierarchy = true;
	m_ShowContentExplorer = true;
	m_ShowJoystickInfo = true;

	m_ShowPlayPauseToolbar = true;
	m_ShowLightsToolbar = false;
	m_ShowVolumesToolbar = false;
	m_ShowLandscapeToolbar = false;
	m_ShowFoliageToolbar = false;
	m_ShowMultiplayerToolbar = false;
	m_ShowSaveOpenToolbar = false;
	m_ShowTargetPlatformToolbar = false;

	m_ContentExplorer = nullptr;
}

void MainDockSpace::OnAttach()
{
	PROFILE_FUNCTION();

	Fonts::LoadFonts();

	Settings::SetDefaultBool("Windows", "Viewport", m_ShowViewport);
	Settings::SetDefaultBool("Windows", "Console", m_ShowConsole);
	Settings::SetDefaultBool("Windows", "ContentExplorer", m_ShowContentExplorer);
	Settings::SetDefaultBool("Windows", "JoystickInfo", m_ShowJoystickInfo);
	Settings::SetDefaultBool("Windows", "Hierarchy", m_ShowHierarchy);
	Settings::SetDefaultBool("Windows", "Properties", m_ShowProperties);
	Settings::SetDefaultBool("Windows", "ErrorList", m_ShowErrorList);
	Settings::SetDefaultBool("Windows", "EditorPreferences", m_ShowEditorPreferences);
	Settings::SetDefaultBool("Windows", "ProjectSettings", m_ShowProjectSettings);

	m_ShowViewport = Settings::GetBool("Windows", "Viewport");
	m_ShowConsole = Settings::GetBool("Windows", "Console");
	m_ShowEditorPreferences = Settings::GetBool("Windows", "EditorPreferences");
	m_ShowProjectSettings = Settings::GetBool("Windows", "ProjectSettings");
	m_ShowContentExplorer = Settings::GetBool("Windows", "ContentExplorer");
	m_ShowJoystickInfo = Settings::GetBool("Windows", "JoystickInfo");
	m_ShowErrorList = Settings::GetBool("Windows", "ErrorList");
	m_ShowProperties = Settings::GetBool("Windows", "Properties");
	m_ShowHierarchy = Settings::GetBool("Windows", "Hierarchy");

	m_ContentExplorer = new ContentExplorerPanel(&m_ShowContentExplorer);

	Application::Get().AddOverlay(new EditorPreferencesPanel(&m_ShowEditorPreferences));
	Application::Get().AddOverlay(new ProjectSettingsPanel(&m_ShowProjectSettings));
	Application::Get().AddOverlay(m_ContentExplorer);
	Application::Get().AddOverlay(new ConsolePanel(&m_ShowConsole));
	Application::Get().AddOverlay(new JoystickInfoPanel(&m_ShowJoystickInfo));
	HierarchyPanel* hierarchyPanel = new HierarchyPanel(&m_ShowHierarchy);
	Application::Get().AddOverlay(hierarchyPanel);
	Application::Get().AddOverlay(new ViewportPanel(&m_ShowViewport, hierarchyPanel));
	Application::Get().AddOverlay(new PropertiesPanel(&m_ShowProperties, hierarchyPanel));

	if (!Application::GetOpenDocument().empty())
	{
		OpenProject(Application::GetOpenDocument());
	}

	std::string recentProjectsStr = Settings::GetValue("Files", "Recent_Files");

	std::vector<std::string> recentProjectsList = SplitString(recentProjectsStr, ',');

	for (std::filesystem::path project : recentProjectsList)
	{
		if (project.extension() == ".proj")
		{
			m_RecentProjects.push_back(project);
		}
	}
}

void MainDockSpace::OnDetach()
{
	Settings::SetBool("Windows", "EditorPreferences", m_ShowEditorPreferences);
	Settings::SetBool("Windows", "Viewport", m_ShowViewport);
	Settings::SetBool("Windows", "Console", m_ShowConsole);
	Settings::SetBool("Windows", "ContentExplorer", m_ShowContentExplorer);
	Settings::SetBool("Windows", "JoystickInfo", m_ShowJoystickInfo);
	Settings::SetBool("Windows", "Hierarchy", m_ShowHierarchy);
	Settings::SetBool("Windows", "Properties", m_ShowProperties);
	Settings::SetBool("Windows", "ErrorList", m_ShowErrorList);

	Settings::SaveSettings();
}

void MainDockSpace::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<AppOpenDocumentChange>(BIND_EVENT_FN(MainDockSpace::OnOpenProject));
}

void MainDockSpace::OnUpdate(float deltaTime)
{
}

void MainDockSpace::OnImGuiRender()
{
	static bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);//TEMP 

	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become un-docked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &m_Show, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	HandleKeyBoardInputs();

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	else
	{
		CLIENT_ERROR("Docking not enabled.");
	}
	bool about = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem(ICON_FA_FILE" New Scene", "Ctrl + N"))
			{
				m_ContentExplorer->CreateNewScene();
			}
			if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS" New Project", "Ctrl + Shift + N"))
			{
				Application::Get().AddOverlay(new ProjectsStartScreen(true));
			}
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN" Open Project...", "Ctrl + O"))
			{
				std::optional<std::wstring> fileToOpen = FileDialog::Open(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*\0");
				if (fileToOpen)
					Application::Get().SetOpenDocument(fileToOpen.value());
			}
			if (ImGui::BeginMenu(ICON_FA_FOLDER_OPEN" Open Recent"))
			{
				for (auto project : m_RecentProjects)
				{
					if (ImGui::MenuItem(project.filename().string().c_str()))
					{
						Application::SetOpenDocument(project);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(ICON_FA_SAVE" Save Scene", "Ctrl + S", nullptr, SceneManager::IsSceneLoaded()))
			{
				SceneManager::CurrentScene()->Save(false);
			}
			if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT" Exit", "Alt + F4")) Application::Get().Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ICopyable* iCopy = dynamic_cast<ICopyable*>(s_CurrentlyFocusedPanel);

			bool copyable = false;

			if (iCopy != nullptr)
				copyable = !iCopy->IsReadOnly();

			IUndoable* iUndo = dynamic_cast<IUndoable*>(s_CurrentlyFocusedPanel);

			bool undoable = false, redoable = false;
			if (iUndo != nullptr)
			{
				undoable = iUndo->CanUndo();
				redoable = iUndo->CanRedo();
			}

			if (ImGui::MenuItem(ICON_FA_UNDO" Undo", "Ctrl + Z", nullptr, undoable))
				iUndo->Undo();
			if (ImGui::MenuItem(ICON_FA_REDO" Redo", "Ctrl + Y", nullptr, redoable))
				iUndo->Redo();
			ImGui::Separator();//-----------------------------------------------
			if (ImGui::MenuItem(ICON_FA_CUT" Cut", "Ctrl + X", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Cut();
			if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl + C", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Copy();
			if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl + V", nullptr, copyable && ImGui::GetClipboardText() != nullptr))
				iCopy->Paste();
			if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl + D", nullptr, copyable))
				iCopy->Duplicate();
			if (ImGui::MenuItem(ICON_FA_TRASH_ALT" Delete", "Del", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Delete();
			ImGui::Separator();//-----------------------------------------------
			if (ImGui::MenuItem(ICON_FA_MOUSE_POINTER" Select All", "Ctrl + A", nullptr, copyable))
				iCopy->SelectAll();
			ImGui::Separator();//-----------------------------------------------
			ImGui::MenuItem(ICON_FA_COG" Preferences", "", &m_ShowEditorPreferences);
			ImGui::MenuItem(ICON_FA_COGS" Project Settings", "", &m_ShowProjectSettings);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem(ICON_FA_TOOLS" Properties", "", &m_ShowProperties);
			ImGui::MenuItem(ICON_FA_SITEMAP" Hierarchy", "", &m_ShowHierarchy);
			ImGui::MenuItem(ICON_FA_FOLDER_OPEN" Content Explorer", "", &m_ShowContentExplorer);
			ImGui::MenuItem(ICON_FA_BORDER_ALL" Viewport", "", &m_ShowViewport);
			ImGui::MenuItem(ICON_FA_TERMINAL" Console", "", &m_ShowConsole);
			ImGui::MenuItem(ICON_FA_TIMES_CIRCLE" Error List", "", &m_ShowErrorList, false);//TODO: Create Error list panel
			ImGui::MenuItem(ICON_FA_TASKS" Task List", "", &m_ShowTaskList, false);//TODO: Create Task List ImguiPanel
			ImGui::MenuItem(ICON_FA_GAMEPAD" Joystick Info", "", &m_ShowJoystickInfo);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			//TODO: create a toolbar for these tools
			ImGui::MenuItem(ICON_FA_PLAY" Play", "", &m_ShowPlayPauseToolbar); //TODO: create a play/pause tool
			ImGui::MenuItem(ICON_FA_LIGHTBULB" Lights", "", &m_ShowLightsToolbar, false); //TODO: create a lights tool
			ImGui::MenuItem(ICON_FA_DICE_D6" Volumes", "", &m_ShowVolumesToolbar, false); //TODO: Create a Volumes tool e.g. blocking volumes
			ImGui::MenuItem(ICON_FA_MOUNTAIN" Landscape", "", &m_ShowLandscapeToolbar, false); //TODO: create landscape tool
			ImGui::MenuItem(ICON_FA_SEEDLING" Foliage", "", &m_ShowFoliageToolbar, false); //TODO: create a foliage tool
			ImGui::MenuItem(ICON_FA_NETWORK_WIRED" Multiplayer", "", &m_ShowMultiplayerToolbar, false); //TODO: Create multiplayer tool
			ImGui::MenuItem(ICON_FA_SAVE" Save/Open", "", &m_ShowSaveOpenToolbar, false); //TODO: Create Save/openTool
			ImGui::MenuItem(ICON_FA_STEAM" Target Platform", "", &m_ShowTargetPlatformToolbar, false); //TODO: Create target Platform tool
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem(ICON_FA_INFO_CIRCLE" About", "", &about);
			if (ImGui::MenuItem(ICON_FA_BOOK" Documentation", ""))
			{
				Webpage::OpenWebpage(L"https://github.com/ThomasJowett/Cross-Platform-Game-Engine/wiki");
			}
			ImGui::EndMenu();
		}

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x / 2.0f);
		ImGui::Separator();

		if (m_ShowPlayPauseToolbar)
		{
			PlayPauseToolbar::Render();
		}

		ImGui::EndMenuBar();
	}

	if (about) ImGui::OpenPopup("About");

	if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Version: %i.%i.%i", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		ImGui::Text("Built on: %s", __DATE__);
		ImGui::Separator();
		ImGui::Text("Dear ImGui version: %s", ImGui::GetVersion());
		ImGui::Text("spd log version: %i.%i.%i", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
		ImGui::Text("cereal version: %i.%i.%i", CEREAL_VERSION_MAJOR, CEREAL_VERSION_MINOR, CEREAL_VERSION_PATCH);
		ImGui::Text("Simple ini version: 4.17");
		ImGui::Text("entt version: 3.5.2");
		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	ImGui::End();

	//Toolbars------------------------------------------------------------------------------------------------------------

	//if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	//{
	//	if (m_ShowPlayPauseToolbar)
	//	{
	//		PlayPauseToolbar::Render();
	//	}
	//}

	//ImGui::End();
}



void MainDockSpace::OpenProject(const std::filesystem::path& filename)
{

	ENGINE_INFO("Opened Project: {0}", filename.string());

	m_ContentExplorer->SwitchTo(filename);

	std::ifstream file(filename);

	cereal::JSONInputArchive input(file);
	ProjectData data;
	input(data);
	file.close();

	SceneManager::ChangeScene(std::filesystem::path(data.DefaultScene));
}

bool MainDockSpace::OnOpenProject(AppOpenDocumentChange& event)
{
	OpenProject(Application::Get().GetOpenDocument());

	return false;
}

void MainDockSpace::HandleKeyBoardInputs()
{
	ImGuiIO& io = ImGui::GetIO();
	bool shift = io.KeyShift;
	bool ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	bool alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ctrl && !shift && !alt && ImGui::IsKeyPressed('S'))
	{
		SceneManager::CurrentScene()->Save(false);
	}
	else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('N'))
	{
		m_ContentExplorer->CreateNewScene();
	}
	else if (ctrl && shift && !alt && ImGui::IsKeyPressed('N'))
	{
		Application::Get().AddOverlay(new ProjectsStartScreen(true));
	}
	else if (ctrl && !shift && !alt && ImGui::IsKeyPressed('O'))
	{
		std::optional<std::wstring> fileToOpen = FileDialog::Open(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*\0");
		if (fileToOpen)
			Application::Get().SetOpenDocument(fileToOpen.value());
	}
}
