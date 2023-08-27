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

extern VkInstance g_VkInstance;
static std::vector<VkCommandBuffer> s_ImGuiCommandBuffers;

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

			VkDevice device = vulkanContext->GetDevice()->GetVkDevice();
			VkDescriptorPool descriptorPool;

			Ref<VulkanSwapChain> swapChain = vulkanContext->GetSwapChain();

			// Create Descriptor Pool
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
			};
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 100 * IM_ARRAYSIZE(pool_sizes);
			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool));

			ImGui_ImplVulkan_InitInfo initInfo = {};
			initInfo.Instance = g_VkInstance;
			initInfo.PhysicalDevice = vulkanContext->GetPhysicalDevice()->GetVkPhysicalDevice();
			initInfo.Device = device;
			initInfo.QueueFamily = vulkanContext->GetPhysicalDevice()->GetGraphicsQueueFamilyIndex();
			initInfo.Queue = vulkanContext->GetDevice()->GetGraphicsQueue();
			initInfo.PipelineCache = nullptr;
			initInfo.DescriptorPool = descriptorPool;
			initInfo.Subpass = 0;
			initInfo.MinImageCount = 2;
			initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			initInfo.Allocator = nullptr;
			initInfo.ImageCount = swapChain->GetImageCount();
			initInfo.CheckVkResultFn = [](VkResult result) {
				if (result == VK_SUCCESS)
					return;
				ENGINE_ERROR("Vulkan: VkResult = {0}", result);
				if (result < 0)
					abort();
			};
			bool success = ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* user_data) {
				VkInstance instance = static_cast<VkInstance>(user_data);
				return reinterpret_cast<PFN_vkVoidFunction>(vkGetInstanceProcAddr(instance, function_name));
				}, g_VkInstance);
			m_UsingImGui = ImGui_ImplVulkan_Init(&initInfo, swapChain->GetRenderPass());

			{
				VkCommandBuffer commandBuffer = vulkanContext->GetDevice()->GetCommandBuffer(true);

				ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

				vulkanContext->GetDevice()->FlushCommandBuffer(commandBuffer);

				VK_CHECK_RESULT(vkDeviceWaitIdle(device));
				ImGui_ImplVulkan_DestroyFontUploadObjects();
			}

			uint32_t framesInFlight = 3;
			s_ImGuiCommandBuffers.resize(framesInFlight);
			for (uint32_t i = 0; i < framesInFlight; i++)
				s_ImGuiCommandBuffers[i] = vulkanContext->GetDevice()->CreateSecondaryCommandBuffer();
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
		Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

		Ref<VulkanContext> vulkanContext = std::dynamic_pointer_cast<VulkanContext>(context);

		VkDevice device = vulkanContext->GetDevice()->GetVkDevice();

		Ref<VulkanSwapChain> swapChain = vulkanContext->GetSwapChain();

		uint32_t width = swapChain->GetWidth();
		uint32_t height = swapChain->GetHeight();

		uint32_t commandBufferIndex = swapChain->GetCurrentBufferIndex();

		VkClearValue clearValues[2];
		clearValues[0].color = { {0.4f, 0.4, 0.4, 0.4f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkCommandBufferBeginInfo drawCmdBuffInfo = {};
		drawCmdBuffInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		drawCmdBuffInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		drawCmdBuffInfo.pNext = nullptr;
		VkCommandBuffer drawCommandBuffer = swapChain->GetCurrentDrawCommandBuffer();
		VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffer, &drawCmdBuffInfo));

		VkRenderPassBeginInfo renderPassBegininfo = {};
		renderPassBegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBegininfo.pNext = nullptr;
		renderPassBegininfo.renderPass = swapChain->GetRenderPass();
		renderPassBegininfo.framebuffer = swapChain->GetCurrentFramebuffer();
		renderPassBegininfo.renderArea.offset.x = 0;
		renderPassBegininfo.renderArea.offset.y = 0;
		renderPassBegininfo.renderArea.extent.width = width;
		renderPassBegininfo.renderArea.extent.height = height;
		renderPassBegininfo.clearValueCount = 1;
		renderPassBegininfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBegininfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = swapChain->GetRenderPass();
		inheritanceInfo.framebuffer = swapChain->GetCurrentFramebuffer();

		VkCommandBufferBeginInfo cmdBuffInfo = {};
		cmdBuffInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBuffInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdBuffInfo.pInheritanceInfo = &inheritanceInfo;

		VK_CHECK_RESULT(vkBeginCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex], &cmdBuffInfo));

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)height;
		viewport.height = -(float)height;
		viewport.width = (float)width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(s_ImGuiCommandBuffers[commandBufferIndex], 0, 1, &scissor);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), drawCommandBuffer);

		VK_CHECK_RESULT(vkEndCommandBuffer(s_ImGuiCommandBuffers[commandBufferIndex]));

		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.push_back(s_ImGuiCommandBuffers[commandBufferIndex]);

		vkCmdExecuteCommands(drawCommandBuffer, uint32_t(commandBuffers.size()), commandBuffers.data());

		vkCmdEndRenderPass(drawCommandBuffer);
		VK_CHECK_RESULT(vkEndCommandBuffer(drawCommandBuffer));
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
