#include "ExampleLayer2D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

ExampleLayer2D::ExampleLayer2D()
	:Layer("Example 2D"),m_CameraController(16.0f/9.0f)
{
}


ExampleLayer2D::~ExampleLayer2D()
{
}

void ExampleLayer2D::OnAttach()
{
	m_VertexArray = VertexArray::Create();

	float vertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f
	};

	unsigned int indices[] = { 0,1,2, 0,2,3 };

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, 6);

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_position"}
	};

	vertexBuffer->SetLayout(layout);

	m_VertexArray->AddVertexBuffer(vertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);

	Ref<Shader> shader = Shader::Create("BasicShader");
}

void ExampleLayer2D::OnDetach()
{
}

void ExampleLayer2D::OnUpdate(float deltaTime)
{
}

void ExampleLayer2D::OnEvent(Event & e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text(std::to_string(m_CameraController.GetZoom()).c_str());
	ImGui::ColorEdit4("Square Colour", m_Colour);
	ImGui::End();
}
