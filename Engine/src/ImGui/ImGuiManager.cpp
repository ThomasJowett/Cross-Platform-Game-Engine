#include "ImGuiManager.h"

#include "Asset/Texture.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "Renderer/RendererAPI.h"
#include <memory>
#include <webgpu/webgpu.hpp>


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_wgpu.h"

#include "Platform/WebGPU/WebGPUContext.h"

#include "GLFW/glfw3.h"

ImGuiManager::ImGuiManager() : m_UsingImGui(false) {}

void ImGuiManager::Init()
{
	PROFILE_FUNCTION();

	// Setup Dear Imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::OpenGL)
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigWindowsMoveFromTitleBarOnly = true;

	m_IniFile = std::filesystem::path(Application::GetWorkingDirectory() / "imgui.ini").string();

	io.IniFilename = m_IniFile.c_str();

	// Setup Platform/Renderer bindings
	if (api == RendererAPI::API::OpenGL)
	{
		GLFWwindow* window = Application::GetWindow()->GetNativeWindow();

		if (ImGui_ImplGlfw_InitForOpenGL(window, true))
			m_UsingImGui = ImGui_ImplOpenGL3_Init("#version 460");
	}
	else if (api == RendererAPI::API::WebGPU)
	{
		GLFWwindow* window = Application::GetWindow()->GetNativeWindow();

		if (ImGui_ImplGlfw_InitForOther(window, true))
		{
			Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

			Ref<WebGPUContext> webGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

			auto device = webGPUContext->GetWebGPUDevice();
			auto format = webGPUContext->GetSwapchainFormat();

			ImGui_ImplWGPU_InitInfo init_info = {};
			init_info.Device = device;
			init_info.NumFramesInFlight = 3;
			init_info.RenderTargetFormat = format;
			init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;

			m_UsingImGui = ImGui_ImplWGPU_Init(&init_info);
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

	// Rendering
	ImGui::Render();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else if (api == RendererAPI::API::WebGPU)
	{
		Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
		if (!context)
			return;
		Ref<WebGPUContext> webGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);
		if (!webGPUContext)
			return;

		wgpu::TextureView targetView = webGPUContext->GetCurrentTextureView();
		if (!targetView)
			return;

		ImDrawData* drawData = ImGui::GetDrawData();
		if (!drawData)
			return;

		wgpu::RenderPassColorAttachment colourAttachment = {};
		colourAttachment.view = targetView;
		colourAttachment.loadOp = wgpu::LoadOp::Clear;
		colourAttachment.storeOp = wgpu::StoreOp::Store;
		colourAttachment.clearValue = { 0.1f, 0.1f, 0.1f, 1.0f }; // TODO: set the clear colour properly

		wgpu::RenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &colourAttachment;
		renderPassDesc.depthStencilAttachment = nullptr;

		auto device = webGPUContext->GetWebGPUDevice();
		if (!device)
			return;
		wgpu::CommandEncoderDescriptor encoderDesc = {};
		encoderDesc.label = "ImGui Command Encoder";
		wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);
		if (!encoder)
			return;

		wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);
		if (renderPass) {
			ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
			renderPass.end();
		}

		wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
		cmdBufferDescriptor.label = "ImGui Command Buffer";
		wgpu::CommandBuffer command = encoder.finish(cmdBufferDescriptor);

		if (command)
			webGPUContext->GetQueue().submit(1, &command);
	}

	if (api == RendererAPI::API::OpenGL && (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
	{
		GLFWwindow* backup_current_context = nullptr;
		backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
