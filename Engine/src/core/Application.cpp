#include "stdafx.h"
#include "Application.h"

#include <GLFW/glfw3.h>

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
	m_window = std::unique_ptr<Window>(Window::Create());
	m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}


Application::~Application()
{
}

void Application::Run()
{
	while (m_running)
	{
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		m_window->OnUpdate();
	}
}

void Application::OnEvent(Event & e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	std::cout << e << std::endl;
}

bool Application::OnWindowClose(WindowCloseEvent & e)
{
	m_running = false;
	return true;
}
