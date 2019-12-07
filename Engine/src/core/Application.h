#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "imgui/ImGuiLayer.h"

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	void OnEvent(Event& e);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	static inline Application& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);

private:
	std::unique_ptr<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	LayerStack m_LayerStack;

	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();