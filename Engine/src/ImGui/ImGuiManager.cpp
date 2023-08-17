#include "stdafx.h"
#include "ImGuiManager.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "Renderer/Texture.h"
#include "Renderer/RendererAPI.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "imgui/backends/imgui_impl_vulkan.h"
#include "Platform/Vulkan/VulkanContext.h"

static ImGui_ImplVulkanH_Window g_WindowData;
extern VkInstance g_VkInstance;

#include "GLFW/glfw3.h"

#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11RendererAPI.h"
#include "Platform/DirectX/DirectX11Context.h"
#include "imgui/backends/imgui_impl_dx11.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;
#endif // _WINDOWS

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
	if (api == RendererAPI::API::Directx11)
	{
#ifdef _WINDOWS
		if (ImGui_ImplGlfw_InitForOther(std::any_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow()), true))
		{
			m_UsingImGui = ImGui_ImplDX11_Init(g_D3dDevice, g_ImmediateContext);
		}
#endif // _WINDOWS
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());

		if (ImGui_ImplGlfw_InitForOpenGL(window, true))
			m_UsingImGui = ImGui_ImplOpenGL3_Init("#version 460");
	}
	else if (api == RendererAPI::API::Vulkan)
	{
		GLFWwindow* window = std::any_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());

		if (ImGui_ImplGlfw_InitForVulkan(window, true)) {
			Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

			Ref<VulkanContext> vulkanContext = std::dynamic_pointer_cast<VulkanContext>(context);
			VkDescriptorPool descriptorPool;

			ImGui_ImplVulkanH_Window* wd = &g_WindowData;
			ImGui_ImplVulkan_InitInfo initInfo = {};
			initInfo.Instance = g_VkInstance;
			initInfo.PhysicalDevice = vulkanContext->GetPhysicalDevice()->GetVkPhysicalDevice();
			initInfo.Device = vulkanContext->GetDevice()->GetVkDevice();
			initInfo.QueueFamily = vulkanContext->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex();
			initInfo.Queue = vulkanContext->GetDevice()->GetGraphicsQueue();
			//initInfo.PipelineCache = g_PipelineCache;
			//initInfo.DescriptorPool = g_DescriptorPool;
			initInfo.Subpass = 0;
			//initInfo.MinImageCount = g_MinImageCount;
			initInfo.ImageCount = wd->ImageCount;
			initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			//initInfo.Allocator = g_Allocator;
			initInfo.CheckVkResultFn = [](VkResult result) {
				if (result == 0)
					return;
				ENGINE_ERROR("Vulkan: VkResult = {0}", result);
				if (result < 0)
					abort();
			};
			bool success = ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* user_data) {
				VkInstance instance = static_cast<VkInstance>(user_data);
				return reinterpret_cast<PFN_vkVoidFunction>(vkGetInstanceProcAddr(instance, function_name));
				}, g_VkInstance);
			m_UsingImGui = ImGui_ImplVulkan_Init(&initInfo, wd->RenderPass);
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
	if (api == RendererAPI::API::Directx11)
	{
#ifdef _WINDOWS
		ImGui_ImplDX11_Shutdown();
#endif // _WINDOWS
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
	if (api == RendererAPI::API::Directx11)
	{
#ifdef _WINDOWS
		ImGui_ImplDX11_NewFrame();
#endif // _WINDOWS
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}
	else if (api == RendererAPI::API::Vulkan)
	{
		ImGui_ImplVulkan_NewFrame();
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
	if (api == RendererAPI::API::Directx11)
	{
#ifdef _WINDOWS
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // _WINDOWS
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else if (api == RendererAPI::API::Vulkan)
	{
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), );
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
