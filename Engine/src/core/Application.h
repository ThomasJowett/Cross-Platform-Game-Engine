#pragma once

#include "core.h"
#include "Events/Event.h"
#include "Window.h"

class DLLIMPEXP_CORE Application
{
public:
	Application();
	virtual ~Application();

	void Run();
private:
	std::unique_ptr<Window> m_window;
	bool m_running = true;
};

// To be defined in CLIENT
Application* CreateApplication();