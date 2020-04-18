#include "stdafx.h"
#include "Application.h"

#include "GLFW/glfw3.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Core/Joysticks.h"

#include "Logging/Logger.h"

Application* Application::s_Instance = nullptr;

Application::Application(const WindowProps& props)
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications");
	s_Instance = this;
	m_Window = Scope<Window>(Window::Create(props));
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	Renderer::Init();

	Joysticks::SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	m_ImGuiLayer = new ImGuiLayer();

	PushOverlay(m_ImGuiLayer);
}


Application::~Application()
{
}

void Application::Run()
{
	PROFILE_FUNCTION();

	const double deltaTime = 0.01f; // fixed update delta time of 10ms seconds (100 times a second)

	double currentTime = glfwGetTime();
	double accumulator = 0.0f;

	while (m_Running)
	{
		PROFILE_SCOPE("Run Loop");

		OnUpdate();

		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= deltaTime)
		{
			PROFILE_SCOPE("Layer Stack Fixed Update");

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnFixedUpdate();
				}
			}
			accumulator -= deltaTime;
		}

		{
			PROFILE_SCOPE("Layer Stack Update");

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate((float)frameTime);
				}
			}
		}

		if (m_ImGuiLayer->IsUsing())
		{
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
		}
		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event& e)
{
	PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

	dispatcher.Dispatch<JoystickConnected>([](JoystickConnected& event)
		{
			ENGINE_INFO("Controller Connected");
			return false;
		});
	dispatcher.Dispatch<JoystickDisconnected>([](JoystickDisconnected& event)
		{
			ENGINE_INFO("Controller Disonnected");
			return false;
		});

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
	{
		(*it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Application::PushLayer(Layer* layer)
{
	PROFILE_FUNCTION();

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
	PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	PROFILE_FUNCTION();

	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
		return false;
	}
	m_Minimized = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
	return false;
}