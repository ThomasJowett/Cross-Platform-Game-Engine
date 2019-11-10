#include "stdafx.h"
#include "ImGuiLayer.h"

#include "Core/Application.h"

#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

//TEMPORARY
#include "GLFW/glfw3.h"
#include "GLAD/glad.h"

ImGuiLayer::ImGuiLayer()
	:Layer("ImGui")
{
}


ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::OnAttach()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	//TODO: use built in keycodes
	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	ImGui_ImplOpenGL3_Init("#version 460");
}

void ImGuiLayer::OnDetach()
{
}

void ImGuiLayer::OnUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

	float time = (float)glfwGetTime();
	io.DeltaTime = m_time > 0.0f ? (time = m_time) : (1.0f/60.0f);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event & event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
	dispatcher.Dispatch<MouseMotionEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseMotionEvent));
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(ImGuiLayer::OnMouseWheelEvent));
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
	dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
	dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(ImGuiLayer::OnWindowResize));
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = true;
	return false;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[e.GetMouseButton()] = false;
	return false;
}

bool ImGuiLayer::OnMouseMotionEvent(MouseMotionEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(e.GetX(), e.GetY());
	return false;
}

bool ImGuiLayer::OnMouseWheelEvent(MouseWheelEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += e.GetXOffset();
	io.MouseWheel += e.GetYOffset();
	return false;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[e.GetKeyCode()] = true;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	return false;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[e.GetKeyCode()] = false;
	return false;
}

bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	int keycode = e.GetKeyCode();
	if (keycode > 0 && keycode < 0x10000)
		io.AddInputCharacter((unsigned short)keycode);

	return false;
}

bool ImGuiLayer::OnWindowResize(WindowResizeEvent & e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	
	glViewport(0, 0, e.GetWidth(), e.GetHeight());
	return false;
}
