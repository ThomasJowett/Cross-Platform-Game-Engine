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

	glGenVertexArrays(1, &m_vertexarray);
	glBindVertexArray(m_vertexarray);

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

	float vertices[3 * 3] =
	{
		-0.6f, -0.4f, 0.0f,
		 0.4f, -0.6f, 0.0f,
		  0.f,  0.6f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	unsigned int indices[3] = { 0,1,2 };
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}


Application::~Application()
{
}

void Application::Run()
{
	while (m_running)
	{
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(m_vertexarray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

		for each(Layer* layer in m_layerStack)
		{
			layer->OnUpdate();
		}

		m_ImGuiLayer->Begin();
		for each(Layer* layer in m_layerStack)
		{
			layer->OnImGuiRender();
		}
		m_ImGuiLayer->End();

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
