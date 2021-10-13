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

/* ------------------------------------------------------------------------------------------------------------------ */

struct LineVertex
{
	Vector3f ClipCoord;
	Colour colour;
	Vector2f TexCoords;
	float Width;
	float Length;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct Renderer2DData
{
	const uint32_t MaxQuads = 10000;
	const uint32_t MaxVertices = MaxQuads * 4;
	const uint32_t MaxIndices = MaxQuads * 6;
	static const size_t MaxTexturesSlots = 32; //TODO query the hardware to calculate the maximum number of textures

	const uint32_t MaxLines = 10000;
	const uint32_t MaxLineVertices = MaxLines * 4;
	const uint32_t MaxLineIndices = MaxLines * 6;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> QuadShader;
	Ref<Texture> WhiteTexture;

	Ref<VertexArray> LineVertexArray;
	Ref<VertexBuffer> LineVertexBuffer;
	Ref<Shader> LineShader;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	uint32_t LineIndexCount = 0;
	LineVertex* LineVertexBufferBase = nullptr;
	LineVertex* LineVertexBufferPtr = nullptr;

	std::array<Ref<Texture>, MaxTexturesSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	Vector3f QuadVertexPositions[4];

	Matrix4x4 ViewProjectionMatrix;
	uint32_t ScreenWidth = 1920, ScreenHeight = 1080;

	Renderer2D::Stats Statistics;
};

/* ------------------------------------------------------------------------------------------------------------------ */

static Renderer2DData s_Data;

bool Renderer2D::Init()
{
	PROFILE_FUNCTION();

	// Quads --------------------------------------------------------------------------------------------

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

	s_Data.QuadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Colour"},
			{ShaderDataType::Float2, "a_Texcoord"},
			{ShaderDataType::Float, "a_TexIndex"}
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

	// Lines --------------------------------------------------------------------------------------------

	s_Data.LineVertexArray = VertexArray::Create();
	s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxLineVertices * sizeof(LineVertex));

	s_Data.LineVertexBuffer->SetLayout({
		{ShaderDataType::Float3, "a_clipCoord"},
		{ShaderDataType::Float4, "a_colour"},
		{ShaderDataType::Float2, "a_texcoord"},
		{ShaderDataType::Float, "a_width"},
		{ShaderDataType::Float, "a_height"}
		});
	s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
	s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxLineVertices];

	uint32_t* lineIndices = new uint32_t[s_Data.MaxLineIndices];
	offset = 0;
	for (uint32_t i = 0; i < s_Data.MaxLineIndices; i += 6)
	{
		lineIndices[i + 0] = offset + 0;
		lineIndices[i + 1] = offset + 1;
		lineIndices[i + 2] = offset + 2;
		
		lineIndices[i + 3] = offset + 2;
		lineIndices[i + 4] = offset + 3;
		lineIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> lineIndexBuffer = IndexBuffer::Create(lineIndices, s_Data.MaxLineIndices);
	s_Data.LineVertexArray->SetIndexBuffer(lineIndexBuffer);
	delete[] lineIndices;

	// Textures & Shaders -------------------------------------------------------------------------------

	s_Data.WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	int32_t samplers[s_Data.MaxTexturesSlots];
	for (uint32_t i = 0; i < s_Data.MaxTexturesSlots; i++)
		samplers[i] = i;

	s_Data.QuadShader = Shader::Create("Texture");
	s_Data.QuadShader->Bind();
	s_Data.QuadShader->SetIntArray("u_Textures", samplers, s_Data.MaxTexturesSlots);

	s_Data.LineShader = Shader::Create("Line");
	s_Data.LineShader->Bind();
	s_Data.LineShader->SetInt("u_Caps", 3);

	// set the texture slot at [0] to white texture
	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f };
	s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f };
	s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f };
	s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f };


	return s_Data.QuadShader == nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::Shutdown()
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::OnWindowResize(uint32_t width, uint32_t height)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::BeginScene(const Matrix4x4& transform, const Matrix4x4& projection)
{
	PROFILE_FUNCTION();
	s_Data.ViewProjectionMatrix = projection * Matrix4x4::Inverse(transform);
	s_Data.QuadShader->Bind();
	s_Data.QuadShader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix, true);

	StartBatch();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::EndScene()
{
	PROFILE_FUNCTION();

	Flush();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::Flush()
{
	if (s_Data.QuadIndexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
	s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
	{
		s_Data.TextureSlots[i]->Bind(i);
	}
	s_Data.QuadVertexBuffer->Bind();
	s_Data.QuadShader->Bind();
	s_Data.QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

	//s_Data.LineVertexBuffer->Bind();
	//s_Data.LineShader->Bind();
	//s_Data.LineVertexArray->Bind();
	//RenderCommand::DrawIndexed(s_Data.LineVertexArray, s_Data.LineIndexCount);

	s_Data.Statistics.DrawCalls++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::StartBatch()
{
	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, rotation, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const float& rotation, const Colour& colour, float tilingFactor)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, subtexture, rotation, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	DrawQuad(transform, texture, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const float& rotation, const Colour& colour, float tilingFactor)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	DrawQuad(transform, subtexture, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const float& rotation, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, rotation, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const float& rotation, const Colour& colour)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	DrawQuad(transform, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, 0.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, subtexture, 0.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour)
{
	DrawQuad(position, size, texture, 0.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour)
{
	DrawQuad(position, size, subtexture, 0.0f, colour);
}
/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, 0.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Colour& colour)
{
	DrawQuad(position, size, 0.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Colour& colour)
{
	PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
	{
		NextBatch();
	}

	const Vector2f texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f,1.0f} , {0.0f,1.0f} };

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->colour = colour;
		s_Data.QuadVertexBufferPtr->TexCoords = texCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = 0.0f;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Ref<Texture2D>& texture, const Colour& colour, float tilingFactor)
{
	if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
	{
		NextBatch();
	}

	float textureIndex = 0.0f;
	const Vector2f texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f,1.0f} , {0.0f,1.0f} };

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
		if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTexturesSlots)
			NextBatch();

		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->colour = colour;
		s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * texCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Ref<SubTexture2D>& subtexture, const Colour& colour, float tilingFactor)
{
	PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
	{
		NextBatch();
	}

	float textureIndex = 0.0f;
	const Vector2f* texCoords = subtexture->GetTextureCoordinates();

	for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
	{
		if (*s_Data.TextureSlots[i].get() == *subtexture->GetTexture().get())
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTexturesSlots)
			NextBatch();

		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = subtexture->GetTexture();
		s_Data.TextureSlotIndex++;
	}


	for (size_t i = 0; i < 4; i++)
	{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->colour = colour;
		s_Data.QuadVertexBufferPtr->TexCoords = tilingFactor * texCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawSprite(const Matrix4x4& transform, const SpriteComponent& spriteComp)
{
	if (spriteComp.Texture)
	{
		DrawQuad(transform, spriteComp.Texture, spriteComp.Tint, spriteComp.TilingFactor);
	}
	else
	{
		DrawQuad(transform, spriteComp.Tint);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawLine(const Vector2f& start, Vector2f& end, const float& thickness, const Colour& colour)
{
	if (s_Data.LineIndexCount >= s_Data.MaxLineIndices)
		NextBatch();

	//world to clip
	Vector3f clipI = s_Data.ViewProjectionMatrix * Vector3f(start.x, start.y, 0.0f);
	Vector3f clipJ = s_Data.ViewProjectionMatrix * Vector3f(end.x, end.x, 0.0f);

	//clip to pixel
	Vector2f pixelStart, pixelEnd;
	pixelStart.x = 0.5f * (float)s_Data.ScreenWidth * (clipI.x / 1.0f + 1.0f);
	pixelStart.y = 0.5f * (float)s_Data.ScreenHeight * (1.0f - clipI.y / 1.0f);
	pixelEnd.x = 0.5f * (float)s_Data.ScreenWidth * (clipJ.x / 1.0f + 1.0f);
	pixelEnd.y = 0.5f * (float)s_Data.ScreenHeight * (1.0f - clipJ.y / 1.0f);

	Vector2f direction = pixelEnd - pixelStart;
	float lineLength = direction.Magnitude();

	if (lineLength < 1e-10) return;

	direction = direction / lineLength;
	Vector2f normal(-direction.y, +direction.x);

	float d = 0.5f * thickness;

	float dOverWidth = d / (float)s_Data.ScreenWidth;
	float dOverHeight = d / (float)s_Data.ScreenHeight;

	Vector3f offset;

	offset.x = (-direction.x + normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.LineVertexBufferPtr->ClipCoord = clipI + offset;
	s_Data.LineVertexBufferPtr->colour = colour;
	s_Data.LineVertexBufferPtr->TexCoords = { -d, +d };
	s_Data.LineVertexBufferPtr->Width = 2 * d;
	s_Data.LineVertexBufferPtr->Length = lineLength;
	s_Data.LineVertexBufferPtr++;

	offset.x = (+direction.x + normal.x) * dOverWidth;
	offset.y = (-direction.y - normal.y) * dOverHeight;
	s_Data.LineVertexBufferPtr->ClipCoord = clipI + offset;
	s_Data.LineVertexBufferPtr->colour = colour;
	s_Data.LineVertexBufferPtr->TexCoords = { lineLength + d, +d };
	s_Data.LineVertexBufferPtr->Width = 2 * d;
	s_Data.LineVertexBufferPtr->Length = lineLength;
	s_Data.LineVertexBufferPtr++;

	offset.x = (+direction.x - normal.x) * dOverWidth;
	offset.y = (-direction.y + normal.y) * dOverHeight;
	s_Data.LineVertexBufferPtr->ClipCoord = clipI + offset;
	s_Data.LineVertexBufferPtr->colour = colour;
	s_Data.LineVertexBufferPtr->TexCoords = { lineLength + d, -d };
	s_Data.LineVertexBufferPtr->Width = 2 * d;
	s_Data.LineVertexBufferPtr->Length = lineLength;
	s_Data.LineVertexBufferPtr++;

	offset.x = (-direction.x - normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.LineVertexBufferPtr->ClipCoord = clipI + offset;
	s_Data.LineVertexBufferPtr->colour = colour;
	s_Data.LineVertexBufferPtr->TexCoords = { -d, -d };
	s_Data.LineVertexBufferPtr->Width = 2 * d;
	s_Data.LineVertexBufferPtr->Length = lineLength;
	s_Data.LineVertexBufferPtr++;

	offset.x = (-direction.x + normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.LineVertexBufferPtr->ClipCoord = clipI + offset;
	s_Data.LineVertexBufferPtr->colour = colour;
	s_Data.LineVertexBufferPtr->TexCoords = { -d, +d };
	s_Data.LineVertexBufferPtr->Width = 2 * d;
	s_Data.LineVertexBufferPtr->Length = lineLength;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineIndexCount += 6;

	s_Data.Statistics.QuadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawLine(const Vector2f& start, Vector2f& end, const Colour& colour)
{
	DrawLine(start, end, 1.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const float& thickness, const Colour& colour)
{
	//TODO draw polyline
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const Colour& colour)
{
	DrawPolyline(points, 1.0f, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

const Renderer2D::Stats& Renderer2D::GetStats()
{
	return s_Data.Statistics;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::ResetStats()
{
	memset(&s_Data.Statistics, 0, sizeof(Stats));
}
