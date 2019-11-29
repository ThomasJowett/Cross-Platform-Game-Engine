#include "stdafx.h"
#include "Application.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include <GLAD/glad.h>

Application* Application::s_Instance = nullptr;

// Temporary function
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:		return GL_FLOAT;
	case ShaderDataType::Float2:	return GL_FLOAT;
	case ShaderDataType::Float3:	return GL_FLOAT;
	case ShaderDataType::Float4:	return GL_FLOAT;
	case ShaderDataType::Mat3:		return GL_FLOAT;
	case ShaderDataType::Mat4:		return GL_FLOAT;
	case ShaderDataType::Int:		return GL_INT;
	case ShaderDataType::Int2:		return GL_INT;
	case ShaderDataType::Int3:		return GL_INT;
	case ShaderDataType::Int4:		return GL_INT;
	case ShaderDataType::Bool:		return GL_BOOL;
	default:
		break;
	}

	CORE_ASSERT(false, "Shader Data type not recognised");
	return 0;
}

Application::Application()
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications")
	s_Instance = this;
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	glGenVertexArrays(1, &m_VertexArray);
	glBindVertexArray(m_VertexArray);

	float vertices[3 * 7] =
	{
		-0.6f, -0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		 0.4f, -0.6f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		  0.f,  0.6f, 0.0f, 0.0f,  1.0f, 0.0f,  1.0f
	};

	unsigned int indices[3] = { 0,1,2 };

	m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
	m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_position"},
		{ShaderDataType::Float4, "a_colour"}
	};

	m_VertexBuffer->SetLayout(layout);

	uint32_t index = 0;
	for (const auto& element : m_VertexBuffer->GetLayout())
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, element.Count(), 
			ShaderDataTypeToOpenGLBaseType(element.Type), 
			element.Normalized ? GL_TRUE : GL_FALSE, 
			layout.GetStride(), 
			(const void*)element.Offset);
		index++;
	}

	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_position;
		layout(location = 1) in vec4 a_colour;

		out vec3 v_position;
		out vec4 v_colour;

		void main()
		{
			v_position = a_position;
			gl_Position = vec4(a_position, 1.0);
			v_colour = a_colour;
		}
	)";

	std::string fragmentSrc = R"(
		#version 330 core

		in vec3 v_position;
		in vec4 v_colour;

		layout(location = 0) out vec4 frag_colour;

		void main()
		{
			frag_colour = vec4(v_colour);
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
