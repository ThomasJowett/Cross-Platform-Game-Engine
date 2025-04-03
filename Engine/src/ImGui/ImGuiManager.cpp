#include "stdafx.h"
#include "ImGuiManager.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "Asset/Texture.h"
#include "Renderer/RendererAPI.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_wgpu.h"

#include "Platform/WebGPU/WebGPUContext.h"

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

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	m_IniFile = std::filesystem::path(Application::GetWorkingDirectory() / "imgui.ini").string();

	io.IniFilename = m_IniFile.c_str();

	//Setup Platform/Renderer bindings
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::OpenGL)
	{
		GLFWwindow* window = Application::GetWindow()->GetNativeWindow();

		if (ImGui_ImplGlfw_InitForOpenGL(window, true))
			m_UsingImGui = ImGui_ImplOpenGL3_Init("#version 460");
	}
	else if (api == RendererAPI::API::WebGPU)
	{
		GLFWwindow* window = Application::GetWindow()->GetNativeWindow();

		if (ImGui_ImplGlfw_InitForOther(window, true)) {
			Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

			Ref<WebGPUContext> webGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

			auto device = webGPUContext->GetWebGPUDevice();
			auto format = webGPUContext->GetSwapchainFormat();

			m_UsingImGui = ImGui_ImplWGPU_Init(device, 3, format);//TODO: get the device, swapchain format and depth texture format
		}
	}
	else
	{
		ENGINE_CRITICAL("ImGui not available for this graphics API");
	}
}

void ImGuiManager::Shutdown()
{
	PROFILE_FUNCTION();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::WebGPU)
	{
		ImGui_ImplWGPU_Shutdown();
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	m_UsingImGui = false;
}

void ImGuiManager::OnEvent(Event& event)
{
	ImGuiIO& io = ImGui::GetIO();
	event.Handled |= event.IsInCategory(EventCategory::MOUSE) & io.WantCaptureMouse;
	event.Handled |= event.IsInCategory(EventCategory::KEYBOARD) & io.WantCaptureKeyboard;
}

void ImGuiManager::Begin()
{
	PROFILE_FUNCTION();
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}
	else if (api == RendererAPI::API::WebGPU)
	{
		ImGui_ImplWGPU_NewFrame();
	}
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
}

void ImGuiManager::End()
{
	PROFILE_FUNCTION();
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2((float)app.GetWindow()->GetWidth(), (float)app.GetWindow()->GetHeight());

	//Rendering
	ImGui::Render();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else if (api == RendererAPI::API::WebGPU)
	{
		//ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);//TODO: pass the render pass to this function
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = nullptr;
		backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
