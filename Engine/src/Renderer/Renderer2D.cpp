#include "stdafx.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

struct QuadVertex
{
	Vector3f Position;
	Colour colour;
	Vector2f TexCoords;
	float TexIndex;
};

struct Renderer2DData
{
	const uint32_t MaxQuads = 10000;
	const uint32_t MaxVertices = MaxQuads * 4;
	const uint32_t MaxIndices = MaxQuads * 6;
	static const size_t MaxTexturesSlots = 32; //TODO query the hardware to calculate the maximum number of textures

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> Shader;
	Ref<Texture> WhiteTexture;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	std::array<Ref<Texture>, MaxTexturesSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	Vector3f QuadVertexPositions[4];

	Renderer2D::Stats Statistics;
};

static Renderer2DData s_Data;

bool Renderer2D::Init()
{
	PROFILE_FUNCTION();

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

	s_Data.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_position"},
			{ShaderDataType::Float4, "a_colour"},
			{ShaderDataType::Float2, "a_texcoord"},
			{ShaderDataType::Float, "a_texIndex"}
		});

	s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

	s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

	uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
	s_Data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
	delete[] quadIndices;


	s_Data.WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	int32_t samplers[s_Data.MaxTexturesSlots];
	for (uint32_t i = 0; i < s_Data.MaxTexturesSlots; i++)
		samplers[i] = i;

	s_Data.Shader = Shader::Create("Texture");
	s_Data.Shader->Bind();
	s_Data.Shader->SetIntArray("u_textures", samplers, s_Data.MaxTexturesSlots);

	// set the texture slot at [0] to white texture
	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f };
	s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f };
	s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f };
	s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f };


	return s_Data.Shader == nullptr;
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
{
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	PROFILE_FUNCTION();
	s_Data.Shader->Bind();
	s_Data.Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix(), true);

	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.QuadIndexCount = 0;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::EndScene()
{
	PROFILE_FUNCTION();
	uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
	s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
	Flush();
}

void Renderer2D::Flush()
{
	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
	{
		s_Data.TextureSlots[i]->Bind(i);
	}
	RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	s_Data.Statistics.DrawCalls++;
}

void Renderer2D::FlushAndReset()
{
	EndScene();

	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.QuadIndexCount = 0;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, rotation, colour, tilingFactor);
}

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
	{
		FlushAndReset();
	}

	float textureIndex = 0.0f;

	for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
	{
		if (*s_Data.TextureSlots[i].get() == *texture.get())
		{
			textureIndex = (float)i;
			break;
		}
	}
	if (textureIndex == 0.0f)
	{
		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * Vector2f( 0.0f, 0.0f );
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * Vector2f(1.0f, 0.0f );
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * Vector2f(1.0f, 1.0f );
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * Vector2f(0.0f, 1.0f );
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const float& rotation, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, rotation, colour);
}

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const float& rotation, const Colour& colour)
{
	PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
	{
		FlushAndReset();
	}

	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = { 0.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = 0.0f;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = { 1.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = 0.0f;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = { 1.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = 0.0f;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->colour = colour;
	s_Data.QuadVertexBufferPtr->TexCoords = { 0.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = 0.0f;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(position, size, texture, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, 0.0f, colour);
}

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Colour& colour)
{
	DrawQuad(position, size, 0.0f, colour);
}

void Renderer2D::DrawLine(const Vector2f& start, Vector2f& end, const float& thickness, const Colour& colour)
{
	//TODO: draw line
}

void Renderer2D::DrawLine(const Vector2f& start, Vector2f& end, const Colour& colour)
{
	DrawLine(start, end, 1.0f, colour);
}

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const float& thickness, const Colour& colour)
{
	//TODO draw polyline
}

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const Colour& colour)
{
	DrawPolyline(points, 1.0f, colour);
}

const Renderer2D::Stats& Renderer2D::GetStats()
{
	return s_Data.Statistics;
}

void Renderer2D::ResetStats()
{
	memset(&s_Data.Statistics, 0, sizeof(Stats));
}
