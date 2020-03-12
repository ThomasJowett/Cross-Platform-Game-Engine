#include "stdafx.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

static const size_t MaxQuadCount = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static const size_t MaxTextures = 32; //TODO query the hardware to calculate the maximum number of textures

struct Vertex
{
	Vector3f Position;
	Colour colour;
	Vector2f TexCoords;
	float TexIndex;
};

struct RendererData
{

};

struct Renderer2DStorage
{
	Ref<VertexArray> vertexArray;
	Ref<Shader> shader;
	Ref<Texture> whiteTexture;
};

static Renderer2DStorage* s_Data;

bool Renderer2D::Init()
{
	PROFILE_FUNCTION();
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

	s_Data->whiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data->whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	s_Data->shader = Shader::Create("Texture");
	s_Data->shader->Bind();
	s_Data->shader->SetInt("u_texture", 0);

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
	PROFILE_FUNCTION();
	s_Data->shader->Bind();
	s_Data->shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(), true);
}

void Renderer2D::EndScene()
{
	PROFILE_FUNCTION();
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const Ref<Texture2D>& texture, const float & rotation, const Colour& colour, float tilingFactor)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, rotation, colour, tilingFactor);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const Ref<Texture2D>& texture, const float & rotation, const Colour& colour, float tilingFactor)
{
	PROFILE_FUNCTION();
	s_Data->shader->SetFloat4("u_colour", colour);
	s_Data->shader->SetFloat("u_tilingFactor", tilingFactor);
	texture->Bind();

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	s_Data->shader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const float & rotation, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, rotation, colour);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const float & rotation, const Colour& colour)
{
	PROFILE_FUNCTION();
	s_Data->shader->SetFloat4("u_colour", colour);
	s_Data->shader->SetFloat("u_tilingFactor", 1.0f);
	s_Data->whiteTexture->Bind();

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	s_Data->shader->SetMat4("u_ModelMatrix", transform, true);
	s_Data->vertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->vertexArray);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(position, size, texture, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector2f & position, const Vector2f & size, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector3f & position, const Vector2f & size, const Colour& colour)
{
	DrawQuad(position, size, 0.0f, colour);
}

void Renderer2D::DrawLine(const Vector2f & start, Vector2f & end, const float & thickness, const Colour & colour)
{
	//TODO: draw line
}

void Renderer2D::DrawLine(const Vector2f & start, Vector2f & end, const Colour & colour)
{
	DrawLine(start, end, 1.0f, colour);
}

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const float & thickness, const Colour & colour)
{
	//TODO draw polyline
}

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const Colour & colour)
{
	DrawPolyline(points, 1.0f, colour);
}
