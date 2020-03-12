#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "imgui/ImGuiLayer.h"

int main(int argc, char* argv[]);

class Application
{
public:
	Application(const WindowProps& props);
	virtual ~Application();

	void OnEvent(Event& e);
	virtual void OnUpdate() = 0;

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	static inline Application& Get() { return *s_Instance; }
	static Window& GetWindow() { return Get().GetWindowImpl(); }

private:
	inline Window& GetWindowImpl() { return *m_Window; }
	void Run();
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	Scope<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	bool m_Minimized = false;
	LayerStack m_LayerStack;

	static Application* s_Instance;
	friend int::main(int argc, char* argv[]);
};

// To be defined in CLIENT
Application* CreateApplication();