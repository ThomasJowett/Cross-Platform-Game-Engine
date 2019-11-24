#include "stdafx.h"
#include "Application.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include <GLAD/glad.h>

Application* Application::s_Instance = nullptr;

Application::Application()
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications")
	s_Instance = this;
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);

	float vertices[3 * 3] =
	{
		-0.6f, -0.4f, 0.0f,
		 0.4f, -0.6f, 0.0f,
		  0.f,  0.6f, 0.0f
	};

	unsigned int indices[3] = { 0,1,2 };

	m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
	m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_position;

		out vec3 v_position;

		void main()
		{
			v_position = a_position;
			gl_Position = vec4(a_position, 1.0);
		}
	)";

	std::string fragmentSrc = R"(
		#version 330 core

		in vec3 v_position;
		layout(location = 0) out vec4 frag_colour;

		void main()
		{
			frag_colour = vec4(v_position * 0.5 + 0.5, 1.0);
		}
	)";

	m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
}


Application::~Application()
{
}

void Application::Run()
{
	while (m_Running)
	{
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Shader->Bind();
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

		for each(Layer* layer in m_LayerStack)
		{
			layer->OnUpdate();
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
