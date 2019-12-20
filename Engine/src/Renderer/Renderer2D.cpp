#include "stdafx.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

struct Renderer2DStorage
{
	Ref<VertexArray> vertexArray;
	Ref<Shader> flatColourShader;
	Ref<Shader> textureShader;
};

static Renderer2DStorage* s_Data;

bool Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();

	s_Data->vertexArray = VertexArray::Create();

	float vertices[] =
	{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = { 0,1,2, 0,2,3 };

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, 6);

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_position"},
		{ShaderDataType::Float2, "a_texcoord"}
	};

	vertexBuffer->SetLayout(layout);

	s_Data->vertexArray->AddVertexBuffer(vertexBuffer);
	s_Data->vertexArray->SetIndexBuffer(indexBuffer);

	s_Data->flatColourShader = Shader::Create("FlatColour");
	s_Data->textureShader = Shader::Create("Texture");
	s_Data->textureShader->Bind();
	s_Data->textureShader->SetInt("u_texture", 0);

	return s_Data->flatColourShader == nullptr;
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
	s_Data->flatColourShader->Bind();
	s_Data->flatColourShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(), true);
	
	s_Data->textureShader->Bind();
	s_Data->textureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(), true);
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
	s_Data->flatColourShader->Bind();
	s_Data->flatColourShader->SetFloat4("u_colour", colour[0], colour[1], colour[2], colour[3]);

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::Scale(Vector3f(size.x, size.y, 1.0f));
	s_Data->flatColourShader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const float & rotation, const float * colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, rotation, colour);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const float & rotation, const float * colour)
{
	s_Data->flatColourShader->Bind();
	s_Data->flatColourShader->SetFloat4("u_colour", colour[0], colour[1], colour[2], colour[3]);

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale(Vector3f(size.x, size.y, 1.0f));
	s_Data->flatColourShader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const Ref<Texture2D>& texture)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const Ref<Texture2D>& texture)
{
	s_Data->textureShader->Bind();
	texture->Bind();

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::Scale(Vector3f(size.x, size.y, 1.0f));
	s_Data->flatColourShader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const float & rotation, const Ref<Texture2D>& texture)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, rotation, texture);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const float & rotation, const Ref<Texture2D>& texture)
{
	s_Data->textureShader->Bind();
	texture->Bind();

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale(Vector3f(size.x, size.y, 1.0f));
	s_Data->flatColourShader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}
