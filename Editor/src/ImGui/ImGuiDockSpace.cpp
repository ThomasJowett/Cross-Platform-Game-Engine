#include "ImGuiDockSpace.h"

#include "imgui/imgui.h"
#include  "Core/Version.h"
#include "Fonts/Fonts.h"

ImGuiDockSpace::ImGuiDockSpace()
	:Layer("Dockspace")
{
	m_Show = true;
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

	Settings::SetDefaultBool("Windows", "Console", false);
	Settings::SetDefaultBool("Windows", "ContentExplorer", true);
	Settings::SetDefaultBool("Windows", "JoystickInfo", false);
	Settings::SetDefaultBool("Windows", "Hierachy", true);
	Settings::SetDefaultBool("Windows", "Properties", true);
	Settings::SetDefaultBool("Windows", "ErrorList", true);

	m_ShowConsole = Settings::GetBool("Windows", "Console");
	m_ShowContentExplorer = Settings::GetBool("Windows", "ContentExplorer");
	m_ShowJoystickInfo = Settings::GetBool("Windows", "JoystickInfo");
	m_ShowErrorList = Settings::GetBool("Windows", "ErrorList");
	m_ShowProperties = Settings::GetBool("Windows", "Properties");
	m_ShowHierachy = Settings::GetBool("Windows", "Hierachy");
}

void ImGuiDockSpace::OnDetach()
{
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

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			ImGui::MenuItem("New Scene", "Ctrl + N");
			ImGui::MenuItem("New Project", "Ctrl + Shift + N");
			ImGui::MenuItem("Open Project", "Ctrl + O");
			ImGui::MenuItem("Save", "Ctrl + S");
			if(ImGui::MenuItem("Exit", "Alt + F4")) Application::Get().Close();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Undo", "Ctrl + Z");
			ImGui::MenuItem("Redo", "Ctrl + Y");
			ImGui::MenuItem("Cut", "Ctrl + X");
			ImGui::MenuItem("Copy", "Ctrl + C");
			ImGui::MenuItem("Paste", "Ctrl + V");
			ImGui::MenuItem("Duplicate", "Ctrl + D");
			ImGui::MenuItem("Preferences", "");
			ImGui::MenuItem("Project Settings", "");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Properties", "", &m_ShowProperties);
			ImGui::MenuItem("Heirachy", "", &m_ShowHierachy);
			ImGui::MenuItem("Content Explorer", "", &m_ShowContentExplorer);
			ImGui::MenuItem("View port", "");
			ImGui::MenuItem("Conosole", "", &m_ShowConsole);
			ImGui::MenuItem("Error List", "", &m_ShowErrorList);
			ImGui::MenuItem("Task List", "", &m_ShowTaskList);
			ImGui::MenuItem("Joystick Info", "", &m_ShowJoystickInfo);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Play", "");
			ImGui::MenuItem("Lights", "");
			ImGui::MenuItem("Volumes", "");
			ImGui::MenuItem("Landscape", "");
			ImGui::MenuItem("Foliage", "");
			ImGui::MenuItem("Multiplayer", "");
			ImGui::MenuItem("Save/Open", "");
			ImGui::MenuItem("Target Platform", "");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", "");
			ImGui::MenuItem("Documentation", "");
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}
