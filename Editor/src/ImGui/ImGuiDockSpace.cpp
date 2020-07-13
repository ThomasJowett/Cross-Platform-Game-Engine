#include "ImGuiDockSpace.h"

#include "imgui/imgui.h"
#include  "Core/Version.h"
#include "Fonts/Fonts.h"


#include "ImGui/ImGuiContentExplorer.h"
#include "ImGui/ImGuiConsole.h"
#include "ImGui/ImGuiJoystickInfo.h"
#include "ImGui/ImGuiContentExplorer.h"
#include "ImGui/ImGuiPreferences.h"
#include "ImGui/ImGuiViewport.h"

#include "Interfaces/ICopyable.h"
#include "Interfaces/IUndoable.h"
#include "Interfaces/ISaveable.h"

Layer* ImGuiDockSpace::s_CurrentlyFoccusedPanel;

ImGuiDockSpace::ImGuiDockSpace()
	:Layer("Dockspace")
{
	m_Show = true;

	m_ShowEditorPreferences = false;
	m_ShowViewport = true;
	m_ShowConsole = true;
	m_ShowErrorList = false;
	m_ShowTaskList = false;
	m_ShowProperties = false;
	m_ShowHierachy = false;
	m_ShowContentExplorer = true;
	m_ShowJoystickInfo = true;
}

void ImGuiDockSpace::OnAttach()
{
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

	Application::Get().AddOverlay(new ImGuiViewportPanel(&m_ShowViewport));
	Application::Get().AddOverlay(new ImGuiEditorPreferences(&m_ShowEditorPreferences));
	Application::Get().AddOverlay(new ImGuiContentExplorer(&m_ShowContentExplorer));
	Application::Get().AddOverlay(new ImGuiConsole(&m_ShowConsole));
	Application::Get().AddOverlay(new ImGuiJoystickInfo(&m_ShowJoystickInfo));
}

void ImGuiDockSpace::OnDetach()
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

void ImGuiDockSpace::OnEvent(Event& event)
{
}

void ImGuiDockSpace::OnUpdate(float deltaTime)
{
}

void ImGuiDockSpace::OnImGuiRender()
{
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

			ImGui::MenuItem("New Scene", "Ctrl + N", nullptr, false);
			ImGui::MenuItem("New Project", "Ctrl + Shift + N", nullptr, false);
			ImGui::MenuItem("Open Project", "Ctrl + O", nullptr, false);
			if (ImGui::MenuItem("Save", "Ctrl + S", nullptr, saveable))
				iSave->Save();
			if (ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Close();
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

			if (ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, undoable))
				iUndo->Undo();
			if (ImGui::MenuItem("Redo", "Ctrl + Y", nullptr, redoable))
				iUndo->Redo();
			ImGui::Separator();//-----------------------------------------------
			if (ImGui::MenuItem("Cut", "Ctrl + X", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Cut();
			if (ImGui::MenuItem("Copy", "Ctrl + C", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Copy();
			if (ImGui::MenuItem("Paste", "Ctrl + V", nullptr, copyable && ImGui::GetClipboardText() != nullptr))
				iCopy->Paste();
			if (ImGui::MenuItem("Duplicate", "Ctrl + D", nullptr, copyable))
				iCopy->Duplicate();
			if (ImGui::MenuItem("Delete", "Del", nullptr, copyable && iCopy->HasSelection()))
				iCopy->Delete();
			ImGui::Separator();//-----------------------------------------------
			if (ImGui::MenuItem("Select All", "Ctrl + A", nullptr, copyable))
				iCopy->SelectAll();
			ImGui::Separator();//-----------------------------------------------
			ImGui::MenuItem("Preferences", "", &m_ShowEditorPreferences);
			ImGui::MenuItem("Project Settings", "", nullptr, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Properties", "", &m_ShowProperties, false);//TODO: Create properties panel
			ImGui::MenuItem("Heirachy", "", &m_ShowHierachy, false);//TODO: create heirachy panel
			ImGui::MenuItem("Content Explorer", "", &m_ShowContentExplorer);
			ImGui::MenuItem("Viewport", "", &m_ShowViewport);
			ImGui::MenuItem("Conosole", "", &m_ShowConsole);
			ImGui::MenuItem("Error List", "", &m_ShowErrorList, false);//TODO: Create Error list panel
			ImGui::MenuItem("Task List", "", &m_ShowTaskList, false);//TODO: Create Tasklist ImguiPanel
			ImGui::MenuItem("Joystick Info", "", &m_ShowJoystickInfo);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			//TODO: create a toolbar for these tools
			ImGui::MenuItem("Play", "", nullptr, false); //TODO: create a play/pause tool
			ImGui::MenuItem("Lights", "", nullptr, false); //TODO: create a lights tool
			ImGui::MenuItem("Volumes", "", nullptr, false); //TODO: Create a Volumes tool e.g. blocking volumes
			ImGui::MenuItem("Landscape", "", nullptr, false); //TODO: create landscape tool
			ImGui::MenuItem("Foliage", "", nullptr, false); //TODO: create a foliage tool
			ImGui::MenuItem("Multiplayer", "", nullptr, false); //TODO: Create multiplayer tool
			ImGui::MenuItem("Save/Open", "", nullptr, false); //TODO: Create Save/openTool
			ImGui::MenuItem("Target Platform", "", nullptr, false); //TODO: Create target Platform tool
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", "", &about);
			if (ImGui::MenuItem("Documentation", ""))
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
}
