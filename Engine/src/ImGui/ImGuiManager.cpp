#include "stdafx.h"
#include "ImGuiManager.h"

#include "Core/Application.h"

#include "Renderer/RendererAPI.h"

#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11RendererAPI.h"
#include "Platform/DirectX/DirectX11Context.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;
#endif // __WINDOWS__

#include "ImGuiConsole.h"

#include "Renderer/Texture.h"

//TEMPORARY
//TODO: remove dependencies on GLAD and GLFW
#include "glad/glad.h"
#include "GLFW/glfw3.h"

ImGuiManager::ImGuiManager()
	:m_UsingImGui(false)
{
}

void ImGuiManager::Init()
{
	PROFILE_FUNCTION();

	// Setup Dear Imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	m_IniFile = std::filesystem::path(Application::GetWorkingDirectory() / "imgui.ini").string();

	io.IniFilename = m_IniFile.c_str();

	//Setup Platform/Renderer bindings
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		HWND windowHandle = std::any_cast<HWND>(Application::GetWindow().GetNativeWindow());

		if (ImGui_ImplWin32_Init(windowHandle))
		{
			m_UsingImGui = ImGui_ImplDX11_Init(g_D3dDevice, g_ImmediateContext);
		}
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		
		if (ImGui_ImplGlfw_InitForOpenGL(window, true))
			m_UsingImGui = ImGui_ImplOpenGL3_Init("#version 460");
	}
	else
	{
		ENGINE_CRITICAL("Could not initialise ImGui");
	}
}

void ImGuiManager::Shutdown()
{
	PROFILE_FUNCTION();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	ImGui::DestroyContext();

	m_UsingImGui = false;
}

void ImGuiManager::OnEvent(Event& event)
{
	ImGuiIO& io = ImGui::GetIO();
	event.Handled |= event.IsInCategory(EventCategory::EC_MOUSE) & io.WantCaptureMouse;
	event.Handled |= event.IsInCategory(EventCategory::EC_KEYBOARD) & io.WantCaptureKeyboard;
}

void ImGuiManager::Begin()
{
	PROFILE_FUNCTION();
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	PROFILE_FUNCTION();
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

	//Rendering
	ImGui::Render();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = nullptr;
		if (api == RendererAPI::API::OpenGL)
		{
			backup_current_context = glfwGetCurrentContext();
		}
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		if (api == RendererAPI::API::OpenGL)
		{
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}
