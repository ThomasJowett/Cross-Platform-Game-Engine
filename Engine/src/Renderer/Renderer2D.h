#pragma once

#include "Camera.h"

#include "Texture.h"
#include "Core/Colour.h"

class Renderer2D
{
public:
	static bool Init();
	static void Shutdown();

	static void OnWindowResize(uint32_t width, uint32_t height);
	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();

	//primitives
	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour colour = { Colour::WHITE });
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const float& rotation = 0.0f, const Colour colour = { Colour::WHITE });

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour colour = { Colour::WHITE });
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const float& rotation = 0.0f, const Colour colour = { Colour::WHITE });

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour colour = { Colour::WHITE });
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Ref<Texture2D>& texture, const Colour colour = { Colour::WHITE });

	static void DrawQuad(const Vector2f& position, const Vector2f& size, const Colour colour = { Colour::WHITE });
	static void DrawQuad(const Vector3f& position, const Vector2f& size, const Colour colour = { Colour::WHITE });

};