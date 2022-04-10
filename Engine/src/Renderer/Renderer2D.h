#pragma once

#include "Camera.h"

#include "Texture.h"
#include "SubTexture2D.h"
#include "Core/Colour.h"

#include "Scene/Components/SpriteComponent.h"
#include "Scene/Components/CircleRendererComponent.h"

class Renderer2D
{
public:
	static bool Init();
	static void Shutdown();

	static void OnWindowResize(uint32_t width, uint32_t height);
	static void BeginScene(const Matrix4x4& transform, const Matrix4x4& projection);
	static void EndScene();

	static void FlushQuads();
	static void FlushCircles();
	static void FlushLines();
	static void FlushHairLines();

	//primitives

	// Quad
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE });
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE });

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Matrix4x4& transform, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawQuad(const Matrix4x4& transform, const Ref<Texture>& texture, const Colour& colour = Colours::WHITE, float tilingFactor = 1.0f, int entityId = -1);
	static void DrawQuad(const Matrix4x4& transform, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE, int entityId = -1);

	// Sprite
	static void DrawSprite(const Matrix4x4& transform, const SpriteComponent& spriteComp, int entityId);

	// Circle
	static void DrawCircle(const Matrix4x4& transform, const Colour& colour, float thickness = 1.0f, float fade = 0.005f, int entityId = -1);
	static void DrawCircle(const Matrix4x4& transform, const CircleRendererComponent& circleComp, int entityId = -1);

	// Line
	static void DrawLine(const Vector2f& start, Vector2f& end, const float& thickness = 1.0f, const Colour& colour = Colours::WHITE);

	// Poly-line
	static void DrawPolyline(const std::vector<Vector2f>& points, const float& thickness = 1.0f, const Colour& colour = Colours::WHITE);

	// Hair Lines
	static void DrawHairLine(const Vector3f& start, const Vector3f& end, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawHairLineRect(const Vector3f& position, Vector2f& size, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawHairLineRect(const Matrix4x4& transform, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawHairLineCircle(const Vector3f& position, float radius, uint32_t segments = 60, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawHairLineCircle(const Matrix4x4& transform, uint32_t segments = 60, const Colour& colour = Colours::WHITE, int entityId = -1);
	static void DrawHairLinePolygon(const std::vector<Vector3f> vertices, const Colour& colour = Colours::WHITE, int entityId = -1);

	struct Stats
	{
		uint32_t drawCalls = 0;
		uint32_t quadCount = 0;
		uint32_t lineCount = 0;

		uint32_t GetTotalVertexCount() { return quadCount * 4; }
		uint32_t GetTotalIndexCount() { return quadCount * 6; }
	};

	static const Stats& GetStats();
	static void ResetStats();

private:
	static void StartQuadsBatch();
	static void StartCirclesBatch();
	static void StartLinesBatch();
	static void StartHairLinesBatch();
	static void NextQuadsBatch();
	static void NextCirclesBatch();
	static void NextLinesBatch();
	static void NextHairLinesBatch();
};