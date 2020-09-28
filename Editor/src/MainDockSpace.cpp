#include "MainDockSpace.h"

#include "imgui/imgui.h"
#include  "Core/Version.h"
#include "Fonts/Fonts.h"
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5Brands.h"
#include "FileSystem/FileDialog.h"

#include "ImGui/ImGuiConsole.h"

#include "Panels/ContentExplorerPanel.h"
#include "Panels/JoystickInfoPanel.h"
#include "Panels/ContentExplorerPanel.h"
#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/HeirachyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Toolbars/PlayPauseToolbar.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

#include "Scene/SceneManager.h"

Layer* MainDockSpace::s_CurrentlyFoccusedPanel;

MainDockSpace::MainDockSpace()
	:Layer("Dockspace")
{
	m_Show = true;

	m_ShowEditorPreferences = false;
	m_ShowViewport = true;
	m_ShowConsole = true;
	m_ShowErrorList = false;
	m_ShowTaskList = false;
	m_ShowProperties = true;
	m_ShowHierachy = true;
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
	Settings::SetDefaultBool("Windows", "Hierachy", m_ShowHierachy);
	Settings::SetDefaultBool("Windows", "Properties", m_ShowProperties);
	Settings::SetDefaultBool("Windows", "ErrorList", m_ShowErrorList);
	Settings::SetDefaultBool("Windows", "EditorPreferences", m_ShowEditorPreferences);

	m_ShowViewport = Settings::GetBool("Windows", "Viewport");
	m_ShowConsole = Settings::GetBool("Windows", "Console");
	m_ShowEditorPreferences = Settings::GetBool("Windows", "EditorPreferences");
	m_ShowContentExplorer = Settings::GetBool("Windows", "ContentExplorer");
	m_ShowJoystickInfo = Settings::GetBool("Windows", "JoystickInfo");
	m_ShowErrorList = Settings::GetBool("Windows", "ErrorList");
	m_ShowProperties = Settings::GetBool("Windows", "Properties");
	m_ShowHierachy = Settings::GetBool("Windows", "Hierachy");

	m_ContentExplorer = new ContentExplorerPanel(&m_ShowContentExplorer);

	Application::Get().AddOverlay(new ViewportPanel(&m_ShowViewport));
	Application::Get().AddOverlay(new EditorPreferencesPanel(&m_ShowEditorPreferences));
	Application::Get().AddOverlay(m_ContentExplorer);
	Application::Get().AddOverlay(new ImGuiConsole(&m_ShowConsole));
	Application::Get().AddOverlay(new JoystickInfoPanel(&m_ShowJoystickInfo));
	HeirachyPanel* heirachyPanel = new HeirachyPanel(&m_ShowHierachy);
	Application::Get().AddOverlay(heirachyPanel);
	Application::Get().AddOverlay(new PropertiesPanel(&m_ShowProperties, heirachyPanel));

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
	Settings::SetBool("Windows", "Hierachy", m_ShowHierachy);
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
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &m_Show, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

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
			ISaveable* iSave = dynamic_cast<ISaveable*>(s_CurrentlyFoccusedPanel);

			bool saveable = iSave != nullptr;

			if(ImGui::MenuItem(ICON_FA_FILE" New Scene", "Ctrl + N"))
			{
				m_ContentExplorer->CreateNewScene();
			}
			ImGui::MenuItem(ICON_FA_FOLDER_PLUS" New Project", "Ctrl + Shift + N", nullptr, false);
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN" Open Project", "Ctrl + O"))
			{
				Application::Get().SetOpenDocument(FileDialog(L"Open Project...", L"Project Files (*.proj)\0*.proj\0Any File\0*.*\0"));
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
			if (ImGui::MenuItem(ICON_FA_SAVE" Save", "Ctrl + S", nullptr, saveable))
				iSave->Save();
			if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT" Exit", "Alt + F4")) Application::Get().Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ICopyable* iCopy = dynamic_cast<ICopyable*>(s_CurrentlyFoccusedPanel);

			bool copyable = false;

			if (iCopy != nullptr)
				copyable = !iCopy->IsReadOnly();

			IUndoable* iUndo = dynamic_cast<IUndoable*>(s_CurrentlyFoccusedPanel);

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
			ImGui::MenuItem(ICON_FA_COGS" Project Settings", "", nullptr, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem(ICON_FA_TOOLS" Properties", "", &m_ShowProperties);
			ImGui::MenuItem(ICON_FA_SITEMAP" Heirachy", "", &m_ShowHierachy);
			ImGui::MenuItem(ICON_FA_FOLDER_OPEN" Content Explorer", "", &m_ShowContentExplorer);
			ImGui::MenuItem(ICON_FA_BORDER_ALL" Viewport", "", &m_ShowViewport);
			ImGui::MenuItem(ICON_FA_TERMINAL" Conosole", "", &m_ShowConsole);
			ImGui::MenuItem(ICON_FA_TIMES_CIRCLE" Error List", "", &m_ShowErrorList, false);//TODO: Create Error list panel
			ImGui::MenuItem(ICON_FA_TASKS" Task List", "", &m_ShowTaskList, false);//TODO: Create Tasklist ImguiPanel
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
#ifdef __WINDOWS__
				ShellExecute(0, 0, L"https://github.com/ThomasJowett/Cross-Platform-Game-Engine/wiki", 0, 0, SW_SHOW);
#elif __linux__
				system("xdg-open https://github.com/ThomasJowett/Cross-Platform-Game-Engine/wiki");
#elif __APPLE__
				system("open https://github.com/ThomasJowett/Cross-Platform-Game-Engine/wiki");
#endif
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
		ImGui::Text("Version: "); ImGui::SameLine(); ImGui::Text(VERSION);
		ImGui::Text("Built on: "); ImGui::SameLine(); ImGui::Text(__DATE__);
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

	//TODO: get default scene filepath from the project file

	std::filesystem::path startSceneFilepath = filename;

	startSceneFilepath.remove_filename();
	startSceneFilepath /= "DefaultScene.scene";

	file.close();

	SceneManager::s_CurrentScene = CreateScope<Scene>(startSceneFilepath);
	SceneManager::s_CurrentScene->Deserialise(false);
}

bool MainDockSpace::OnOpenProject(AppOpenDocumentChange& event)
{
	OpenProject(Application::Get().GetOpenDocument());

	return false;
}
