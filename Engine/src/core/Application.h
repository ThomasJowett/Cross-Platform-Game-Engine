#pragma once

#include "core.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"

class DLLIMPEXP_CORE Application
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
	bool m_running = true;
	LayerStack m_layerStack;

	static Application* s_Instance;
};

// To be defined in CLIENT
Application* CreateApplication();