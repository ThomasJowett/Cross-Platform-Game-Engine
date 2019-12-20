#include "ExampleLayer3D.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "ImGui/imgui.h"

ExampleLayer3D::ExampleLayer3D()
	:Layer("Example"), m_CameraController(16.0f / 9.0f)
{
}

void ExampleLayer3D::OnAttach()
{
	m_VertexArray = VertexArray::Create();

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

	m_VertexArray->AddVertexBuffer(vertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);

	Ref<Shader> shader = m_ShaderLibrary.Load("Texture");
	m_Texture = Texture2D::Create("resources/UVChecker.png");
}

void ExampleLayer3D::OnDetach()
{
}

void ExampleLayer3D::OnUpdate(float deltaTime)
{
	Ref<Shader> shader = m_ShaderLibrary.Get("Texture");
	std::dynamic_pointer_cast<OpenGLShader>(shader)->Bind();

	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInteger("u_texture", 0);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	m_Rotation[0] += 0.001f;
	m_Rotation[1] += 0.001f;
	m_Rotation[2] += 0.001f;

	Renderer::Submit(shader, m_VertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1], m_Position[2] }) 
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	Renderer::EndScene();
}

void ExampleLayer3D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer3D::OnImGuiRender()
{
	ImGui::Begin("Settings 3D");
	ImGui::Text(std::to_string(m_CameraController.GetZoom()).c_str());
	ImGui::DragFloat3("Square Position", m_Position,0.01f);
	ImGui::DragFloat3("Square Rotation", m_Rotation, 0.001f);
	ImGui::End();
}