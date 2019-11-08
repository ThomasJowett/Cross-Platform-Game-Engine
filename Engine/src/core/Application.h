#pragma once

#include "core.h"
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
private:
	bool OnWindowClose(WindowCloseEvent& e);

	std::unique_ptr<Window> m_window;
	bool m_running = true;
};

// To be defined in CLIENT
Application* CreateApplication();