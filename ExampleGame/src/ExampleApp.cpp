#include <include.h>

#include "imgui/imgui.h"

class ExampleLayer :public Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(OrthographicCamera(-16.0f, 16.0f, -9.0f, 9.0f))
	{
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

	void OnUpdate(float deltaTime) override
	{
		//moving the camera with the arrow keys
		if (Input::IsKeyPressed(KEY_TAB))
		{
		}
			m_Camera.SetRotation(m_Camera.GetRotation() + Vector3f( 0.0f, 0.0f, 0.1f  ) * deltaTime);

		if (Input::IsKeyPressed(KEY_LEFT))
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + Vector3f(-1.0f, 0.0f, 0.0f) * deltaTime);
		}

		if (Input::IsKeyPressed(KEY_RIGHT))
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + Vector3f(1.0f , 0.0f, 0.0f) * deltaTime);
		}

		if (Input::IsKeyPressed(KEY_UP))
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + Vector3f(0.0f, 1.0f , 0.0f) * deltaTime);
		}

		if (Input::IsKeyPressed(KEY_DOWN))
		{
			m_Camera.SetPosition(m_Camera.GetPosition() + Vector3f(0.0f, -1.0f , 0.0f) * deltaTime);
		}

		RenderCommand::SetClearColour(0.4f, 0.4f, 0.4f, 1.0f);
		RenderCommand::Clear();

		Renderer::BeginScene(m_Camera);
		Renderer::Submit(m_Shader, m_VertexArray);
		Renderer::EndScene();
	}

	void OnEvent(Event& e) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Hello World");
		ImGui::End();
	}

private:
	Camera m_Camera;
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;
};

class ExampleGame : public Application
{
public:
	ExampleGame()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new ImGuiLayer());
	}

	~ExampleGame()
	{

	}
};

/* Entry Point*/
Application* CreateApplication()
{
	return new ExampleGame();
}