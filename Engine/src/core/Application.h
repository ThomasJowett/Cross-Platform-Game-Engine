#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "imgui/ImGuiLayer.h"

class Application
{
public:
	Application(const WindowProps& props);
	virtual ~Application();

	void Run();

	void OnEvent(Event& e);
	virtual void OnUpdate() = 0;

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	static inline Application& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	Scope<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;

	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();