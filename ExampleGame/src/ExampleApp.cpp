#include <include.h>

#include "imgui/imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

class ExampleLayer :public Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(OrthographicCamera(-16.0f, 16.0f, -9.0f, 9.0f))
	{
		m_VertexArray.reset(VertexArray::Create());

		float vertices[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int indices[] = { 0,1,2, 0,2,3 };

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(indices, 6);

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_position"},
			{ShaderDataType::Float2, "a_texCoord"}
		};

		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		Ref<Shader> shader = m_ShaderLibrary.Load("BasicShader");
		m_Texture = Texture2D::Create("resources/circle.png");

		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInteger("u_texture", 0);
	}

	void OnUpdate(float deltaTime) override
	{
		Ref<Shader> shader = m_ShaderLibrary.Get("BasicShader");
		std::dynamic_pointer_cast<OpenGLShader>(shader)->Bind();

		//moving the camera with the arrow keys
		if (Input::IsKeyPressed(KEY_TAB))
		{
			m_Camera.SetRotation(m_Camera.GetRotation() + Vector3f( 0.0f, 0.0f, 0.1f  ) * deltaTime);
		}

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

		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat4("u_colour", colour[0], colour[1], colour[2], colour[4]);

		Renderer::BeginScene(m_Camera);
		m_Texture->Bind();
		Renderer::Submit(shader, m_VertexArray, Matrix4x4::Scale({ 5.0f, 5.0f, 1.0f }));
		Renderer::EndScene();
	}

	void OnEvent(Event& e) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Colour", colour);
		ImGui::End();
	}

private:
	ShaderLibrary m_ShaderLibrary;
	Camera m_Camera;
	Ref<VertexArray> m_VertexArray;

	Ref<Texture2D> m_Texture;

	float colour[4] = { 1.0f,1.0f,1.0f, 1.0f };
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