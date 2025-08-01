#include "stdafx.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Asset/Shader.h"

#include "RenderCommand.h"
#include "UniformBuffer.h"
#include "Core/Asset.h"

#include "Renderer/UI/MSDFData.h"

struct QuadVertex
{
	Vector3f position;
	Colour colour;
	Vector2f texCoords;
	float texIndex;

	// Editor only
	int EntityId;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct CircleVertex
{
	Vector3f worldPosition;
	Vector3f localPosition;
	Colour colour;
	float thickness;
	float fade;

	// Editor only
	int EntityId;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct LineVertex
{
	Vector3f clipCoord;
	Colour colour;
	Vector2f texCoords;
	float width;
	float length;

	// Editor only
	int EntityId;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct TextVertex
{
	Vector3f position;
	Colour colour;
	Vector2f texCoords;
	float texIndex;

	int EntityId;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct HairLineVertex
{
	Vector3f position;
	Colour colour;

	// Editor only
	int EntityId;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct Renderer2DData
{
	const uint32_t maxQuads = 10000;
	const uint32_t maxVertices = maxQuads * 4;
	const uint32_t maxIndices = maxQuads * 6;
	static const size_t maxTexturesSlots = 32; //TODO query the hardware to calculate the maximum number of textures

	const uint32_t maxLines = 10000;
	const uint32_t maxLineVertices = maxLines * 4;
	const uint32_t maxLineIndices = maxLines * 6;

	Ref<VertexBuffer> quadVertexBuffer;
	Ref<IndexBuffer> quadIndexBuffer;
	Ref<Shader> quadShader;
	Ref<Texture> whiteTexture;

	Ref<VertexBuffer> circleVertexBuffer;
	Ref<Shader> circleShader;

	Ref<VertexBuffer> lineVertexBuffer;
	Ref<IndexBuffer> lineIndexBuffer;
	Ref<Shader> lineShader;

	Ref<VertexBuffer> textVertexBuffer;
	Ref<IndexBuffer> textIndexBuffer;
	Ref<Shader> textShader;

	Ref<VertexBuffer> hairLineVertexBuffer;
	Ref<Shader> hairLineShader;

	uint32_t quadIndexCount = 0;
	QuadVertex* quadVertexBufferBase = nullptr;
	QuadVertex* quadVertexBufferPtr = nullptr;

	uint32_t circleIndexCount = 0;
	CircleVertex* circleVertexBufferBase = nullptr;
	CircleVertex* circleVertexBufferPtr = nullptr;

	uint32_t lineIndexCount = 0;
	LineVertex* lineVertexBufferBase = nullptr;
	LineVertex* lineVertexBufferPtr = nullptr;

	uint32_t textIndexCount = 0;
	TextVertex* textVertexBufferBase = nullptr;
	TextVertex* textVertexBufferPtr = nullptr;

	uint32_t hairLineVertexCount = 0;
	HairLineVertex* hairLineVertexBufferBase = nullptr;
	HairLineVertex* hairLineVertexBufferPtr = nullptr;

	std::array<Ref<Texture>, maxTexturesSlots> textureSlots;
	uint32_t textureSlotIndex = 1;

	std::array<Ref<Texture>, maxTexturesSlots> fontAtlasSlots;
	uint32_t fontAtlasSlotIndex = 1;

	Vector3f quadVertexPositions[4];

	uint32_t screenWidth = 1920, screenHeight = 1080;

	Renderer2D::Stats statistics;
};

/* ------------------------------------------------------------------------------------------------------------------ */

static Renderer2DData s_Data;

bool Renderer2D::Init()
{
	PROFILE_FUNCTION();

	// Quads --------------------------------------------------------------------------------------------

	s_Data.quadVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(QuadVertex));

	s_Data.quadVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Colour"},
			{ShaderDataType::Float2, "a_Texcoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Int, "a_EntityId"}
		});

	s_Data.quadVertexBufferBase = new QuadVertex[s_Data.maxVertices];

	uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_Data.maxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	s_Data.quadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.maxIndices);
	delete[] quadIndices;

	// Circles ------------------------------------------------------------------------------------------

	s_Data.circleVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(CircleVertex));

	s_Data.circleVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_WorldPosition"},
			{ShaderDataType::Float3, "a_LocalPosition"},
			{ShaderDataType::Float4, "a_Colour"},
			{ShaderDataType::Float, "a_Thickness"},
			{ShaderDataType::Float, "a_Fade"},
			{ShaderDataType::Int, "a_EntityId"}
		});

	s_Data.circleVertexBufferBase = new CircleVertex[s_Data.maxVertices];

	// Lines --------------------------------------------------------------------------------------------

	s_Data.lineVertexBuffer = VertexBuffer::Create(s_Data.maxLineVertices * sizeof(LineVertex));

	s_Data.lineVertexBuffer->SetLayout({
		{ShaderDataType::Float3, "a_clipCoord"},
		{ShaderDataType::Float4, "a_colour"},
		{ShaderDataType::Float2, "a_texcoord"},
		{ShaderDataType::Float, "a_width"},
		{ShaderDataType::Float, "a_height"}
		});
	s_Data.lineVertexBufferBase = new LineVertex[s_Data.maxLineVertices];

	uint32_t* lineIndices = new uint32_t[s_Data.maxLineIndices];
	offset = 0;
	for (uint32_t i = 0; i < s_Data.maxLineIndices; i += 6)
	{
		lineIndices[i + 0] = offset + 0;
		lineIndices[i + 1] = offset + 1;
		lineIndices[i + 2] = offset + 2;

		lineIndices[i + 3] = offset + 2;
		lineIndices[i + 4] = offset + 3;
		lineIndices[i + 5] = offset + 0;

		offset += 4;
	}

	s_Data.lineIndexBuffer = IndexBuffer::Create(lineIndices, s_Data.maxLineIndices);
	delete[] lineIndices;

	// Text ------------------------------------------------------------------------------------------
	s_Data.textVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(TextVertex));

	s_Data.textVertexBuffer->SetLayout({
		{ShaderDataType::Float3, "a_position"},
		{ShaderDataType::Float4, "a_colour"},
		{ShaderDataType::Float2, "a_texcoord"},
		{ShaderDataType::Float, "a_texIndex"},
		{ShaderDataType::Int, "a_EntityId"}
		});
	s_Data.textVertexBufferBase = new TextVertex[s_Data.maxVertices];

	uint32_t* textIndices = new uint32_t[s_Data.maxIndices];
	offset = 0;
	for (uint32_t i = 0; i < s_Data.maxIndices; i += 6)
	{
		textIndices[i + 0] = offset + 0;
		textIndices[i + 1] = offset + 1;
		textIndices[i + 2] = offset + 2;

		textIndices[i + 3] = offset + 2;
		textIndices[i + 4] = offset + 3;
		textIndices[i + 5] = offset + 0;

		offset += 4;
	}

	s_Data.textIndexBuffer = IndexBuffer::Create(textIndices, s_Data.maxIndices);
	delete[] textIndices;

	// Hair Lines ------------------------------------------------------------------------------------

	s_Data.hairLineVertexBuffer = VertexBuffer::Create(s_Data.maxVertices * sizeof(HairLineVertex));

	s_Data.hairLineVertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_LocalPosition"},
			{ShaderDataType::Float4, "a_Colour"},
			{ShaderDataType::Int, "a_EntityId"}
		});

	s_Data.hairLineVertexBufferBase = new HairLineVertex[s_Data.maxVertices];

	// Textures & Shaders -------------------------------------------------------------------------------

	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_Data.whiteTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, false, &whiteTextureData);

	s_Data.quadShader = Shader::Create("Renderer2D_Quad");
	s_Data.circleShader = Shader::Create("Renderer2D_Circle");
	s_Data.lineShader = Shader::Create("Renderer2D_Line");
	s_Data.hairLineShader = Shader::Create("Renderer2D_HairLine");
	s_Data.textShader = Shader::Create("Renderer2D_Text");

	// set the texture slot at [0] to white texture
	s_Data.textureSlots[0] = s_Data.whiteTexture;

	s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f };
	s_Data.quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f };
	s_Data.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f };
	s_Data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f };

	s_Data.fontAtlasSlots[0] = s_Data.whiteTexture;

	return s_Data.quadShader == nullptr;
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

void Renderer2D::BeginScene()
{
	PROFILE_FUNCTION();

	StartQuadsBatch();
	StartCirclesBatch();
	StartLinesBatch();
	StartTextBatch();
	StartHairLinesBatch();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::EndScene()
{
	PROFILE_FUNCTION();

	FlushQuads();
	FlushCircles();
	FlushLines();
	FlushHairLines();
	FlushText();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::FlushQuads()
{
	if (s_Data.quadIndexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
	s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferBase, dataSize);

	for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
	{
		s_Data.textureSlots[i]->Bind(i);
	}
	s_Data.quadVertexBuffer->Bind();
	s_Data.quadIndexBuffer->Bind();
	s_Data.quadShader->Bind();

	RenderCommand::DrawIndexed(s_Data.quadIndexCount, 0U, 0U, false);

	s_Data.quadVertexBuffer->UnBind();
	s_Data.quadIndexBuffer->UnBind();
	s_Data.statistics.drawCalls++;
}

void Renderer2D::FlushCircles()
{
	if (s_Data.circleIndexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.circleVertexBufferPtr - (uint8_t*)s_Data.circleVertexBufferBase);
	s_Data.circleVertexBuffer->SetData(s_Data.circleVertexBufferBase, dataSize);

	s_Data.circleVertexBuffer->Bind();
	s_Data.quadIndexBuffer->Bind();
	s_Data.circleShader->Bind();
	RenderCommand::DrawIndexed(s_Data.circleIndexCount, 0U, 0U, false);
	s_Data.quadIndexBuffer->UnBind();
	s_Data.circleVertexBuffer->UnBind();
	s_Data.statistics.drawCalls++;
}

void Renderer2D::FlushLines()
{
	if (s_Data.lineIndexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.lineVertexBufferPtr - (uint8_t*)s_Data.lineVertexBufferBase);
	s_Data.lineVertexBuffer->SetData(s_Data.lineVertexBufferBase, dataSize);

	s_Data.lineVertexBuffer->Bind();
	s_Data.lineIndexBuffer->Bind();
	s_Data.lineShader->Bind();
	RenderCommand::DrawIndexed(s_Data.lineIndexCount, 0U, 0U, false);
	s_Data.lineVertexBuffer->UnBind();
	s_Data.lineIndexBuffer->UnBind();
	s_Data.statistics.drawCalls++;
}

void Renderer2D::FlushHairLines()
{
	if (s_Data.hairLineVertexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.hairLineVertexBufferPtr - (uint8_t*)s_Data.hairLineVertexBufferBase);
	s_Data.hairLineVertexBuffer->SetData(s_Data.hairLineVertexBufferBase, dataSize);

	s_Data.hairLineVertexBuffer->Bind();
	s_Data.quadIndexBuffer->Bind();
	s_Data.hairLineShader->Bind();
	RenderCommand::DrawLines(s_Data.hairLineVertexCount);
	s_Data.hairLineVertexBuffer->UnBind();
	s_Data.quadIndexBuffer->UnBind();
	s_Data.statistics.drawCalls++;
}

void Renderer2D::FlushText()
{
	if (s_Data.textIndexCount == 0)
		return;
	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.textVertexBufferPtr - (uint8_t*)s_Data.textVertexBufferBase);
	s_Data.textVertexBuffer->SetData(s_Data.textVertexBufferBase, dataSize);

	for (uint32_t i = 0; i < s_Data.fontAtlasSlotIndex; i++)
	{
		if (s_Data.fontAtlasSlots[i])
			s_Data.fontAtlasSlots[i]->Bind(i);
	}

	s_Data.textVertexBuffer->Bind();
	s_Data.textIndexBuffer->Bind();
	s_Data.textShader->Bind();
	RenderCommand::DrawIndexed(s_Data.textIndexCount);
	s_Data.textVertexBuffer->UnBind();
	s_Data.textIndexBuffer->UnBind();
	s_Data.statistics.drawCalls++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::StartQuadsBatch()
{
	s_Data.quadIndexCount = 0;
	s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

	s_Data.textureSlotIndex = 1;
}

void Renderer2D::StartCirclesBatch()
{
	s_Data.circleIndexCount = 0;
	s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;
}

void Renderer2D::StartLinesBatch()
{
	s_Data.lineIndexCount = 0;
	s_Data.lineVertexBufferPtr = s_Data.lineVertexBufferBase;
}

void Renderer2D::StartTextBatch()
{
	s_Data.textIndexCount = 0;
	s_Data.textVertexBufferPtr = s_Data.textVertexBufferBase;
}

void Renderer2D::StartHairLinesBatch()
{
	s_Data.hairLineVertexCount = 0;
	s_Data.hairLineVertexBufferPtr = s_Data.hairLineVertexBufferBase;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::NextQuadsBatch()
{
	FlushQuads();
	StartQuadsBatch();
}

void Renderer2D::NextCirclesBatch()
{
	FlushCircles();
	StartCirclesBatch();
}

void Renderer2D::NextLinesBatch()
{
	FlushLines();
	StartLinesBatch();
}

void Renderer2D::NextTextBatch()
{
	FlushText();
	StartTextBatch();
}

void Renderer2D::NextHairLinesBatch()
{
	FlushHairLines();
	StartHairLinesBatch();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, texture, rotation, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const float& rotation, const Colour& colour)
{
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, subtexture, rotation, colour);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation, const Colour& colour, float tilingFactor)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	DrawQuad(transform, texture, colour, tilingFactor);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const float& rotation, const Colour& colour)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation) * Matrix4x4::Scale({ size.x, size.y, 1.0f });
	DrawQuad(transform, subtexture, colour);
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
	DrawQuad(Vector3f(position.x, position.y, 0.0f), size, subtexture, colour);
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

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Colour& colour, int entityId)
{
	PROFILE_FUNCTION();

	if (s_Data.quadIndexCount >= s_Data.maxIndices)
	{
		NextQuadsBatch();
	}

	const Vector2f texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f,1.0f} , {0.0f,1.0f} };

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.quadVertexBufferPtr->position = transform * s_Data.quadVertexPositions[i];
		s_Data.quadVertexBufferPtr->colour = colour;
		s_Data.quadVertexBufferPtr->texCoords = texCoords[i];
		s_Data.quadVertexBufferPtr->texIndex = 0.0f;
		s_Data.quadVertexBufferPtr->EntityId = entityId;
		s_Data.quadVertexBufferPtr++;
	}

	s_Data.quadIndexCount += 6;

	s_Data.statistics.quadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Ref<Texture>& texture, const Colour& colour, float tilingFactor, int entityId)
{
	if (s_Data.quadIndexCount >= s_Data.maxIndices)
	{
		NextQuadsBatch();
	}

	float textureIndex = 0.0f;
	const Vector2f texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f,1.0f} , {0.0f,1.0f} };

	if (texture) {
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.textureSlotIndex >= Renderer2DData::maxTexturesSlots)
				NextQuadsBatch();

			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = texture;
			s_Data.textureSlotIndex++;
		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.quadVertexBufferPtr->position = transform * s_Data.quadVertexPositions[i];
		s_Data.quadVertexBufferPtr->colour = colour;
		s_Data.quadVertexBufferPtr->texCoords = tilingFactor * texCoords[i];
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->EntityId = entityId;
		s_Data.quadVertexBufferPtr++;
	}

	s_Data.quadIndexCount += 6;

	s_Data.statistics.quadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawQuad(const Matrix4x4& transform, const Ref<SubTexture2D>& subtexture, const Colour& colour, int entityId)
{
	PROFILE_FUNCTION();

	if (!subtexture)
		return;

	if (s_Data.quadIndexCount >= s_Data.maxIndices)
	{
		NextQuadsBatch();
	}

	float textureIndex = 0.0f;
	const Vector2f* texCoords = subtexture->GetTextureCoordinates();

	if (subtexture->GetTexture())
	{
		for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
		{
			if (*s_Data.textureSlots[i].get() == *subtexture->GetTexture().get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.textureSlotIndex >= Renderer2DData::maxTexturesSlots)
				NextQuadsBatch();

			textureIndex = (float)s_Data.textureSlotIndex;
			s_Data.textureSlots[s_Data.textureSlotIndex] = subtexture->GetTexture();
			s_Data.textureSlotIndex++;
		}
	}

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.quadVertexBufferPtr->position = transform * s_Data.quadVertexPositions[i];
		s_Data.quadVertexBufferPtr->colour = colour;
		s_Data.quadVertexBufferPtr->texCoords = texCoords[i];
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->EntityId = entityId;
		s_Data.quadVertexBufferPtr++;
	}

	s_Data.quadIndexCount += 6;

	s_Data.statistics.quadCount++;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawSprite(const Matrix4x4& transform, const SpriteComponent& spriteComp, int entityId)
{
	if (spriteComp.texture)
	{
		DrawQuad(transform, spriteComp.texture, spriteComp.tint, spriteComp.tilingFactor, entityId);
	}
	else
	{
		DrawQuad(transform, spriteComp.tint, entityId);
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawCircle(const Matrix4x4& transform, const Colour& colour, float thickness, float fade, int entityId)
{
	PROFILE_FUNCTION();

	if (s_Data.circleIndexCount >= s_Data.maxIndices)
	{
		NextCirclesBatch();
	}

	for (size_t i = 0; i < 4; i++)
	{
		s_Data.circleVertexBufferPtr->worldPosition = transform * s_Data.quadVertexPositions[i];
		s_Data.circleVertexBufferPtr->localPosition = s_Data.quadVertexPositions[i] * 2.0f;
		s_Data.circleVertexBufferPtr->colour = colour;
		s_Data.circleVertexBufferPtr->thickness = thickness;
		s_Data.circleVertexBufferPtr->fade = fade;
		s_Data.circleVertexBufferPtr->EntityId = entityId;
		s_Data.circleVertexBufferPtr++;
	}

	s_Data.circleIndexCount += 6;

	s_Data.statistics.quadCount++;
}

void Renderer2D::DrawCircle(const Matrix4x4& transform, const CircleRendererComponent& circleComp, int entityId)
{
	DrawCircle(transform, circleComp.colour, circleComp.thickness, circleComp.fade, entityId);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawLine(const Vector2f& start, const Vector2f& end, const float& thickness, const Colour& colour)
{
	if (s_Data.lineIndexCount >= s_Data.maxLineIndices)
		NextLinesBatch();

	////world to clip
	Vector3f clipI;//s_Data.cameraBuffer.viewProjectionMatrix * Vector3f(start.x, start.y, 0.0f);
	Vector3f clipJ;//s_Data.cameraBuffer.viewProjectionMatrix * Vector3f(end.x, end.x, 0.0f);

	//clip to pixel
	Vector2f pixelStart, pixelEnd;
	pixelStart.x = 0.5f * (float)s_Data.screenWidth * (clipI.x / 1.0f + 1.0f);
	pixelStart.y = 0.5f * (float)s_Data.screenHeight * (1.0f - clipI.y / 1.0f);
	pixelEnd.x = 0.5f * (float)s_Data.screenWidth * (clipJ.x / 1.0f + 1.0f);
	pixelEnd.y = 0.5f * (float)s_Data.screenHeight * (1.0f - clipJ.y / 1.0f);

	Vector2f direction = pixelEnd - pixelStart;
	float lineLength = direction.Magnitude();

	if (lineLength < 1e-10) return;

	direction = direction / lineLength;
	Vector2f normal(-direction.y, +direction.x);

	float d = 0.5f * thickness;

	float dOverWidth = d / (float)s_Data.screenWidth;
	float dOverHeight = d / (float)s_Data.screenHeight;

	Vector3f offset;

	offset.x = (-direction.x + normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.lineVertexBufferPtr->clipCoord = clipI + offset;
	s_Data.lineVertexBufferPtr->colour = colour;
	s_Data.lineVertexBufferPtr->texCoords = { -d, +d };
	s_Data.lineVertexBufferPtr->width = 2 * d;
	s_Data.lineVertexBufferPtr->length = lineLength;
	s_Data.lineVertexBufferPtr++;

	offset.x = (+direction.x + normal.x) * dOverWidth;
	offset.y = (-direction.y - normal.y) * dOverHeight;
	s_Data.lineVertexBufferPtr->clipCoord = clipI + offset;
	s_Data.lineVertexBufferPtr->colour = colour;
	s_Data.lineVertexBufferPtr->texCoords = { lineLength + d, +d };
	s_Data.lineVertexBufferPtr->width = 2 * d;
	s_Data.lineVertexBufferPtr->length = lineLength;
	s_Data.lineVertexBufferPtr++;

	offset.x = (+direction.x - normal.x) * dOverWidth;
	offset.y = (-direction.y + normal.y) * dOverHeight;
	s_Data.lineVertexBufferPtr->clipCoord = clipI + offset;
	s_Data.lineVertexBufferPtr->colour = colour;
	s_Data.lineVertexBufferPtr->texCoords = { lineLength + d, -d };
	s_Data.lineVertexBufferPtr->width = 2 * d;
	s_Data.lineVertexBufferPtr->length = lineLength;
	s_Data.lineVertexBufferPtr++;

	offset.x = (-direction.x - normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.lineVertexBufferPtr->clipCoord = clipI + offset;
	s_Data.lineVertexBufferPtr->colour = colour;
	s_Data.lineVertexBufferPtr->texCoords = { -d, -d };
	s_Data.lineVertexBufferPtr->width = 2 * d;
	s_Data.lineVertexBufferPtr->length = lineLength;
	s_Data.lineVertexBufferPtr++;

	offset.x = (-direction.x + normal.x) * dOverWidth;
	offset.y = (+direction.y + normal.y) * dOverHeight;
	s_Data.lineVertexBufferPtr->clipCoord = clipI + offset;
	s_Data.lineVertexBufferPtr->colour = colour;
	s_Data.lineVertexBufferPtr->texCoords = { -d, +d };
	s_Data.lineVertexBufferPtr->width = 2 * d;
	s_Data.lineVertexBufferPtr->length = lineLength;
	s_Data.lineVertexBufferPtr++;

	s_Data.lineIndexCount += 6;

	s_Data.statistics.lineCount++;
}

void Renderer2D::DrawHairLine(const Vector3f& start, const Vector3f& end, const Colour& colour, int entityId)
{
	if (s_Data.hairLineVertexCount >= s_Data.maxLineIndices)
		NextHairLinesBatch();
	s_Data.hairLineVertexBufferPtr->position = start;
	s_Data.hairLineVertexBufferPtr->colour = colour;
	s_Data.hairLineVertexBufferPtr->EntityId = entityId;
	s_Data.hairLineVertexBufferPtr++;

	s_Data.hairLineVertexBufferPtr->position = end;
	s_Data.hairLineVertexBufferPtr->colour = colour;
	s_Data.hairLineVertexBufferPtr->EntityId = entityId;
	s_Data.hairLineVertexBufferPtr++;

	s_Data.hairLineVertexCount += 2;
	s_Data.statistics.hairLineCount++;
}

void Renderer2D::DrawHairLineRect(const Vector3f& position, const Vector2f& size, const Colour& colour, int entityId)
{
	Vector3f p0 = Vector3f(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	Vector3f p1 = Vector3f(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	Vector3f p2 = Vector3f(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	Vector3f p3 = Vector3f(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawHairLine(p0, p1, colour, entityId);
	DrawHairLine(p1, p2, colour, entityId);
	DrawHairLine(p2, p3, colour, entityId);
	DrawHairLine(p3, p0, colour, entityId);
}

void Renderer2D::DrawHairLineRect(const Matrix4x4& transform, const Colour& colour, int entityId)
{
	Vector3f lineVertices[4];
	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * s_Data.quadVertexPositions[i];

	DrawHairLine(lineVertices[0], lineVertices[1], colour, entityId);
	DrawHairLine(lineVertices[1], lineVertices[2], colour, entityId);
	DrawHairLine(lineVertices[2], lineVertices[3], colour, entityId);
	DrawHairLine(lineVertices[3], lineVertices[0], colour, entityId);
}

void Renderer2D::DrawHairLineCircle(const Vector3f& position, float radius, uint32_t segments, const Colour& colour, int entityId)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::Scale(Vector3f(radius, radius, 1.0f));
	DrawHairLineCircle(transform, segments, colour, entityId);
}

void Renderer2D::DrawHairLineCircle(const Matrix4x4& transform, uint32_t segments, const Colour& colour, int entityId)
{
	DrawHairLineArc(transform, 0, 2 * (float)PI, segments, colour, entityId);
}

void Renderer2D::DrawHairLinePolygon(const std::vector<Vector3f> vertices, const Colour& colour, int entityId)
{
	for (uint32_t i = 0; i < vertices.size() - 1; i++)
	{
		DrawHairLine(vertices[i], vertices[i + 1], colour, entityId);
	}
	DrawHairLine(vertices[vertices.size() - 1], vertices[0], colour, entityId);
}

void Renderer2D::DrawHairLineArc(const Vector3f& position, float radius, float start, float end, uint32_t segments, const Colour& colour, int entityId)
{
	Matrix4x4 transform = Matrix4x4::Translate(position) * Matrix4x4::Scale(Vector3f(radius, radius, 1.0f));
	DrawHairLineArc(transform, start, end, segments, colour, entityId);
}

void Renderer2D::DrawHairLineArc(const Matrix4x4& transform, float start, float end, uint32_t segments, const Colour& colour, int entityId)
{
	if (segments < 3)
		return;
	Vector3f previousPoint(sin(start), cos(start), 0.0f);
	Vector3f currentPoint;

	previousPoint = transform * previousPoint;

	float step = abs(end - start) / segments;
	float angle = start;

	for (uint32_t i = 0; i <= segments; ++i)
	{
		currentPoint.x = sin(angle);
		currentPoint.y = cos(angle);
		currentPoint.z = 0.0f;
		currentPoint = transform * currentPoint;

		DrawHairLine(previousPoint, currentPoint, colour, entityId);
		previousPoint = currentPoint;
		angle += step;
	}
}

void Renderer2D::DrawString(const std::string& text, const Ref<Font> font, float maxWidth, const Vector2f& position, const Colour& colour, int entityId)
{
	DrawString(text, font, maxWidth, Matrix4x4::Translate(Vector3f(position.x, position.y, 0.0f)), colour, entityId);
}

void Renderer2D::DrawString(const std::string& text, const Ref<Font> font, float maxWidth, const Vector3f& position, const Colour& colour, int entityId)
{
	DrawString(text, font, maxWidth, Matrix4x4::Translate(position), colour, entityId);
}

void Renderer2D::DrawString(const std::string& text, const Ref<Font> font, float maxWidth, const Vector2f& position, const float& rotation, const Colour& colour, int entityId)
{
	DrawString(text, font, maxWidth, Matrix4x4::Translate(Vector3f(position.x, position.y, 0.0f)) * Matrix4x4::RotateZ(rotation), colour, entityId);
}

void Renderer2D::DrawString(const std::string& text, const Ref<Font> font, float maxWidth, const Vector3f& position, const float& rotation, const Colour& colour, int entityId)
{
	DrawString(text, font, maxWidth, Matrix4x4::Translate(position) * Matrix4x4::RotateZ(rotation), colour, entityId);
}

void Renderer2D::DrawString(const std::string& text, const Ref<Font> font, float maxWidth, const Matrix4x4& transform, const Colour& colour, int entityId)
{
	if (text.empty() || font == nullptr)
		return;

	if (s_Data.textIndexCount >= s_Data.maxIndices)
	{
		NextTextBatch();
	}

	float textureIndex = 0.0f;

	Ref<Texture2D> fontAtlas = font->GetFontAtlas();

	ASSERT(fontAtlas, "Font atlas cannot be null");
	ASSERT(font->GetMSDFData(), "MSDF Data  cannot be null");

	for (uint32_t i = 1; i < s_Data.fontAtlasSlotIndex; i++)
	{
		if (s_Data.fontAtlasSlots[i]->GetFilepath() == fontAtlas->GetFilepath())
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (s_Data.fontAtlasSlotIndex >= Renderer2DData::maxTexturesSlots)
			NextTextBatch();

		textureIndex = (float)s_Data.fontAtlasSlotIndex;
		s_Data.fontAtlasSlots[s_Data.fontAtlasSlotIndex] = fontAtlas;
		s_Data.fontAtlasSlotIndex++;
	}

	const msdf_atlas::FontGeometry& fontGeometry = font->GetMSDFData()->fontGeometry;
	const msdfgen::FontMetrics& metrics = fontGeometry.getMetrics();

	std::vector<int> nextLines;
	double x = 0.0;
	double fsScale = 1 / (metrics.ascenderY - metrics.descenderY);
	double y = -fsScale * metrics.ascenderY;
	int lastSpace = -1;

	std::u32string utf32string;
	utf32string.resize(text.size());
	std::transform(text.begin(), text.end(), utf32string.begin(), [](char c) -> unsigned char {return c; });

	for (int i = 0; i < utf32string.size(); i++)
	{
		char32_t character = utf32string[i];
		if (character == '\n')
		{
			x = 0;
			y -= fsScale * metrics.lineHeight;
			continue;
		}

		auto glyph = fontGeometry.getGlyph(character);
		if (!glyph)
			glyph = fontGeometry.getGlyph('?');
		if (!glyph)
			continue;

		if (character != ' ' && character != '\t')
		{
			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			Vector2f quadMin((float)pl, (float)pb);
			Vector2f quadMax((float)pr, (float)pt);

			quadMin = quadMin * (float)fsScale;
			quadMax = quadMax * (float)fsScale;
			quadMin += Vector2f((float)x, (float)y);
			quadMax += Vector2f((float)x, (float)y);

			if (quadMax.x > maxWidth && lastSpace != -1)
			{
				i = lastSpace;
				nextLines.emplace_back(lastSpace);
				lastSpace = -1;
				x = 0;
				y -= fsScale * metrics.lineHeight;
			}
		}
		else
		{
			lastSpace = i;
		}

		double advance = glyph->getAdvance();
		fontGeometry.getAdvance(advance, character, utf32string[i + 1]);
		x += fsScale * advance;
	}

	x = 0.0;
	fsScale = 1 / (metrics.ascenderY - metrics.descenderY);
	y = 0.0;
	for (int i = 0; i < utf32string.size(); i++)
	{
		char32_t character = utf32string[i];

		if (character == '\n' || std::any_of(nextLines.begin(), nextLines.end(), [&i](int line) { return i == line; }))
		{
			x = 0;
			y -= fsScale * metrics.lineHeight;
			continue;
		}
		if (character == '\t')
		{
			double advance = fontGeometry.getGlyph(' ')->getAdvance();
			x += fsScale * (advance * 4);
			continue;
		}
		auto glyph = fontGeometry.getGlyph(character);
		if (!glyph)
			glyph = fontGeometry.getGlyph('?');
		if (!glyph)
			continue;

		double l, b, r, t;
		glyph->getQuadAtlasBounds(l, b, r, t);

		double pl, pb, pr, pt;
		glyph->getQuadPlaneBounds(pl, pb, pr, pt);

		pl *= fsScale, pb *= fsScale, pr *= fsScale, pt *= fsScale;
		pl += x, pb += y, pr += x, pt += y;

		double texelWidth = 1.0 / fontAtlas->GetWidth();
		double texelHeight = 1.0 / fontAtlas->GetHeight();
		l *= texelWidth, b *= texelHeight, r *= texelWidth, t *= texelHeight;

		s_Data.textVertexBufferPtr->position = transform * Vector3f((float)pl, (float)pb, 0.0f);
		s_Data.textVertexBufferPtr->colour = colour;
		s_Data.textVertexBufferPtr->texCoords = Vector2f((float)l, (float)b);
		s_Data.textVertexBufferPtr->texIndex = textureIndex;
		s_Data.textVertexBufferPtr->EntityId = entityId;
		s_Data.textVertexBufferPtr++;

		s_Data.textVertexBufferPtr->position = transform * Vector3f((float)pr, (float)pb, 0.0f);
		s_Data.textVertexBufferPtr->colour = colour;
		s_Data.textVertexBufferPtr->texCoords = Vector2f((float)r, (float)b);
		s_Data.textVertexBufferPtr->texIndex = textureIndex;
		s_Data.textVertexBufferPtr->EntityId = entityId;
		s_Data.textVertexBufferPtr++;

		s_Data.textVertexBufferPtr->position = transform * Vector3f((float)pr, (float)pt, 0.0f);
		s_Data.textVertexBufferPtr->colour = colour;
		s_Data.textVertexBufferPtr->texCoords = Vector2f((float)r, (float)t);
		s_Data.textVertexBufferPtr->texIndex = textureIndex;
		s_Data.textVertexBufferPtr->EntityId = entityId;
		s_Data.textVertexBufferPtr++;

		s_Data.textVertexBufferPtr->position = transform * Vector3f((float)pl, (float)pt, 0.0f);
		s_Data.textVertexBufferPtr->colour = colour;
		s_Data.textVertexBufferPtr->texCoords = Vector2f((float)l, (float)t);
		s_Data.textVertexBufferPtr->texIndex = textureIndex;
		s_Data.textVertexBufferPtr->EntityId = entityId;
		s_Data.textVertexBufferPtr++;

		s_Data.textIndexCount += 6;

		double advance = glyph->getAdvance();
		fontGeometry.getAdvance(advance, character, utf32string[i + 1]);
		x += fsScale * advance;

		s_Data.statistics.quadCount++;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::DrawPolyline(const std::vector<Vector2f>& points, const float& thickness, const Colour& colour)
{
	//TODO draw polyline
}

/* ------------------------------------------------------------------------------------------------------------------ */

const Renderer2D::Stats& Renderer2D::GetStats()
{
	return s_Data.statistics;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer2D::ResetStats()
{
	memset(&s_Data.statistics, 0, sizeof(Stats));
}