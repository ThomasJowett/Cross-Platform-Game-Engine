#include "stdafx.h"
#include "Application.h"

Application::Application()
{
	m_window = std::unique_ptr<Window>(Window::Create());
}


Application::~Application()
{
}

void Application::Run()
{
	while (m_running)
	{
		m_window->OnUpdate();
	}
}
