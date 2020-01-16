#include "GridLayer.h"


GridLayer::GridLayer()
	:Layer("Grid"), m_CameraController(16.0f / 9.0f, 0.25f * PI, { 0.0f, 0.0f, 1.0f })
{
}

void GridLayer::OnAttach()
{
	m_CubeVertexArray = VertexArray::Create();

	float vertices[] =
	{
		//front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//0
		 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.5f, 0.0f, 1.0f,
		//back face
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,//4
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
		//right face
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f,//8
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
		 //left face
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//12
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
		//top face
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,//16
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
		//bottom face
		 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//20
		  0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f, 0.0f, 1.0f
	};

	unsigned int indices[] =
	{
		0,1,2, 0,2,3,
		4,5,6, 4,6,7,
		8,9,10, 8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	};

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, 36);

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_position"},
		{ShaderDataType::Float2, "a_texCoord"}
	};

	vertexBuffer->SetLayout(layout);

	m_CubeVertexArray->AddVertexBuffer(vertexBuffer);
	m_CubeVertexArray->SetIndexBuffer(indexBuffer);

	Ref<Shader> shader = m_ShaderLibrary.Load("Texture");
	m_Texture = Texture2D::Create("resources/UVChecker.png");
}

void GridLayer::OnDetach()
{
}

void GridLayer::OnUpdate(float deltaTime)
{
	Ref<Shader> shader = m_ShaderLibrary.Get("Texture");
	shader->Bind();

	shader->SetInt("u_texture", 0);

	m_CameraController.OnUpdate(deltaTime);

	std::cout << m_CameraController.GetCamera().GetPosition().to_string() << std::endl;

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1], m_Position[2] }));

	Renderer::EndScene();
}

void GridLayer::OnEvent(Event & e)
{
	m_CameraController.OnEvent(e);
}

void GridLayer::OnImGuiRender()
{
}
