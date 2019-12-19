#include "stdafx.h"
#include "Application.h"

#include "GLFW/glfw3.h"

#include "Renderer/Renderer.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications")
	s_Instance = this;
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Renderer::Init();
}


Application::~Application()
{
}

void Application::Run()
{
	const double deltaTime = 0.01f; // fixed update delta time of 10ms seconds (100 times a second)

	double currentTime = glfwGetTime();
	double accumulator = 0.0f;

	while (m_Running)
	{
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= deltaTime)
		{
			if (!m_Minimized)
			{
				for each(Layer* layer in m_LayerStack)
				{
					layer->OnUpdate((float)deltaTime);
				}
			}
			accumulator -= deltaTime;
		}

		m_ImGuiLayer->Begin();
		for each(Layer* layer in m_LayerStack)
		{
			layer->OnImGuiRender();
		}
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event & e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Application::PushLayer(Layer * layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer * layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent & e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent & e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}
	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}
