#pragma once

#include "core.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/Shader.h"
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
	inline Window& GetWindow() { return *m_window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);

	std::unique_ptr<Window> m_window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_running = true;
	LayerStack m_layerStack;

	static Application* s_Instance;

	unsigned int m_vertexarray, m_vertexbuffer, m_indexBuffer;

	std::unique_ptr<Shader> m_shader;
};

// To be defined in CLIENT
Application* CreateApplication();