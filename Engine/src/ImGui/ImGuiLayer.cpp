#include "stdafx.h"
#include "ImGuiLayer.h"

#include "Core/Application.h"

#include "Renderer/RendererAPI.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11RendererAPI.h"
#include "Platform/DirectX/DirectX11Context.h"
//#include "examples/imgui_impl_dx11.h"
//#include "examples/imgui_impl_win32.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;
#endif // __WINDOWS__

#include "ImGuiConsole.h"

#include "Renderer/Texture.h"

//TEMPORARY
//TODO: remove dependencies on GLAD and GLFW
#include "glad/glad.h"
#include "GLFW/glfw3.h"

ImGuiLayer::ImGuiLayer()
	:Layer("ImGui"), m_UsingImGui(false)
{
}

void ImGuiLayer::OnAttach()
{
	PROFILE_FUNCTION();

	// Setup Dear Imgui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 2.0f;
		//style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		//style.FrameRounding = 2.0f;
		//style.ChildRounding = 2.0f;
		//style.GrabRounding = 2.0f;

/*#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
		// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) ImVec4(0.860f, 0.930f, 0.890f, v)

		style.Colors[ImGuiCol_Text] = TEXT(0.78f);
		style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused ] = BG(0.58f);
		style.Colors[ImGuiCol_TabActive ] = BG(0.58f);
		style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
		style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
		style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
		style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
		style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
		style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
		style.Colors[ImGuiCol_Header] = MED(0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
		style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
		style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
		style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
		// [...]
		style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.73f);

		style.WindowPadding = ImVec2(6, 4);
		style.WindowRounding = 0.0f;
		style.FramePadding = ImVec2(5, 2);
		style.FrameRounding = 3.0f;
		style.ItemSpacing = ImVec2(7, 1);
		style.ItemInnerSpacing = ImVec2(1, 1);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 6.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 16.0f;
		style.GrabMinSize = 20.0f;
		style.GrabRounding = 2.0f;

		style.WindowTitleAlign.x = 0.50f;

		style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
		style.FrameBorderSize = 0.0f;
		style.WindowBorderSize = 1.0f;*/
	}

	//Setup Platform/Renderer bindings
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		HWND windowHandle = std::any_cast<HWND>(Application::GetWindow().GetNativeWindow());

		//if (ImGui_ImplWin32_Init(windowHandle))
		//{
		//	m_UsingImGui = ImGui_ImplDX11_Init(g_D3dDevice, g_ImmediateContext);
		//}
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

void ImGuiLayer::OnDetach()
{
	PROFILE_FUNCTION();

	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		//ImGui_ImplDX11_Shutdown();
		//ImGui_ImplWin32_Shutdown();
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	ImGui::DestroyContext();
}

void ImGuiLayer::OnEvent(Event& event)
{
	ImGuiIO& io = ImGui::GetIO();
	event.Handled |= event.IsInCategory(EventCategory::EC_MOUSE) & io.WantCaptureMouse;
	event.Handled |= event.IsInCategory(EventCategory::EC_KEYBOARD) & io.WantCaptureKeyboard;
}

void ImGuiLayer::OnImGuiRender()
{
	static bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);

	bool p_open = true;

	ImGui::Begin("FPS", &p_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav);

	ImGui::Text("%.1f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void ImGuiLayer::Begin()
{
	PROFILE_FUNCTION();
	RendererAPI::API api = RendererAPI::GetAPI();
	if (api == RendererAPI::API::Directx11)
	{
#ifdef __WINDOWS__
		//ImGui_ImplDX11_NewFrame();
		//ImGui_ImplWin32_NewFrame();
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	ImGui::NewFrame();
}

void ImGuiLayer::End()
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
		//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // __WINDOWS__
	}
	else if (api == RendererAPI::API::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
