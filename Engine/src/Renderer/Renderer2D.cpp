#include "stdafx.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

//TODO: remove the need the include this here
#include "Platform/OpenGL/OpenGLShader.h"

struct Renderer2DStorage
{
	Ref<VertexArray> vertexArray;
	Ref<Shader> shader;
};

static Renderer2DStorage* s_Data;

bool Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();

	s_Data->vertexArray = VertexArray::Create();

	float vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
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

	s_Data->vertexArray->AddVertexBuffer(vertexBuffer);
	s_Data->vertexArray->SetIndexBuffer(indexBuffer);

	s_Data->shader = Shader::Create("FlatColour");

	return s_Data->shader == nullptr;
}

void Renderer2D::Shutdown()
{
	delete s_Data;
}

void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
{
}

void Renderer2D::BeginScene(const OrthographicCamera & camera)
{
	s_Data->shader->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->shader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix(), true);
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->shader)->UploadUniformMat4("u_ModelMatrix", Matrix4x4(), true);
}

void Renderer2D::EndScene()
{
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const float* colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, colour);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const float* colour)
{
	s_Data->shader->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->shader)->UploadUniformFloat4("u_colour", colour[0], colour[1], colour[2], colour[3]);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}
