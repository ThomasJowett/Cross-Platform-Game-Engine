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

	//primitives

	// Quad
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& texture, const float& rotation = 0.0f, const Colour& colour = { Colours::WHITE }, float tilingFactor = 1.0f);

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Colour& colour = Colours::WHITE);

	static void DrawQuad(const Matrix4x4& transform, const Colour& colour = Colours::WHITE);
	static void DrawQuad(const Matrix4x4& transform, const Ref<Texture>& texture, const Colour& colour = Colours::WHITE, float tilingFactor = 1.0f);
	static void DrawQuad(const Matrix4x4& transform, const Ref<SubTexture2D>& subtexture, const Colour& colour = Colours::WHITE, float tilingFactor = 1.0f);

	// Sprite
	static void DrawSprite(const Matrix4x4& transform, const SpriteComponent& spriteComp);

	// Circle
	static void DrawCircle(const Matrix4x4& transform, const Colour& colour, float thickness = 1.0f, float fade = 0.005f);
	static void DrawCircle(const Matrix4x4& transform, const CircleRendererComponent& circleComp);

	// Line
	static void DrawLine(const Vector2f& start, Vector2f& end, const float& thickness = 1.0f, const Colour& colour = Colours::WHITE);
	static void DrawLine(const Vector2f& start, Vector2f& end, const Colour& colour = Colours::WHITE);

	// Poly-line
	static void DrawPolyline(const std::vector<Vector2f>& points, const float& thickness = 1.0f, const Colour& colour = Colours::WHITE);
	static void DrawPolyline(const std::vector<Vector2f>& points, const Colour& colour = Colours::WHITE);

	struct Stats
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};

	static const Stats& GetStats();
	static void ResetStats();

private:
	static void StartQuadsBatch();
	static void StartCirclesBatch();
	static void NextQuadsBatch();
	static void NextCirclesBatch();
};