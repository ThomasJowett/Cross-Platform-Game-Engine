#include "stdafx.h"
#include "Application.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/OrthographicCamera.h"

Application* Application::s_Instance = nullptr;

Application::Application()
	:m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
	CORE_ASSERT(!s_Instance, "Application already exists! Cannot create multiple applications")
	s_Instance = this;
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));


	m_VertexArray.reset(VertexArray::Create());

	float vertices[4 * 7] =
	{
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  1.0f,
		  -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  1.0f
	};

	unsigned int indices[] = { 0,1,2, 0,2,3 };

	std::shared_ptr<VertexBuffer> vertexBuffer;
	vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
	std::shared_ptr<IndexBuffer> indexBuffer;
	indexBuffer.reset(IndexBuffer::Create(indices, 6));

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_position"},
		{ShaderDataType::Float4, "a_colour"}
	};

	vertexBuffer->SetLayout(layout);

	m_VertexArray->AddVertexBuffer(vertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);

	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_position;
		layout(location = 1) in vec4 a_colour;

		uniform mat4 u_ViewProjection;

		out vec3 v_position;
		out vec4 v_colour;

		void main()
		{
			v_position = a_position;
			gl_Position = u_ViewProjection * vec4(a_position, 1.0);
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
		m_Camera.SetPosition(m_Camera.GetPosition() + Vector3f{0.001f, 0.001f, 0.0f});
		m_Camera.SetRotation(m_Camera.GetRotation() + Vector3f{ 0.0f, 0.0f, 0.01f });

		RenderCommand::SetClearColour(0.4f, 0.4f, 0.4f, 1.0f);
		RenderCommand::Clear();

		Renderer::BeginScene(m_Camera);
		Renderer::Submit(m_Shader, m_VertexArray);
		Renderer::EndScene();

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
