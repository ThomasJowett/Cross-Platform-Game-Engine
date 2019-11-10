#include "stdafx.h"
#include "Application.h"

#include <GLAD/glad.h>

Application* Application::s_Instance = nullptr;

Application::Application()
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications")
	s_Instance = this;
	m_window = std::unique_ptr<Window>(Window::Create());
	m_window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}


Application::~Application()
{
}

void Application::Run()
{
	while (m_running)
	{
		glClearColor(0.4, 0.4, 0.4, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		for each(Layer* layer in m_layerStack)
		{
			layer->OnUpdate();
		}

		m_window->OnUpdate();
	}
}

void Application::OnEvent(Event & e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

	for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Application::PushLayer(Layer * layer)
{
	m_layerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer * layer)
{
	m_layerStack.PushOverlay(layer);
	layer->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent & e)
{
	m_running = false;
	return true;
}
