#pragma once

#include "RendererAPI.h"

class RenderCommand
{
public:
	// Create a Renderer API object
	static void CreateRendererAPI();

	// Initialise the Renderer
	inline static bool Init()
	{
		return s_RendererAPI->Init();
	}

	// Sets the clear colour of the screen or currently bound frame buffer
	inline static void SetClearColour(const Colour& colour)
	{
		s_RendererAPI->SetClearColour(colour);
	}

	// Sets the area of the screen that will be rendered to
	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		return s_RendererAPI->SetViewport(x, y, width, height);
	}

	// Resets the pixels of the screen back to the clear colour
	inline static void Clear()
	{
		s_RendererAPI->Clear();
	}

	// Draws primitives from the vertex array
	inline static void DrawIndexed(uint32_t indexCount = 0, uint32_t startIndex = 0, uint32_t vertexOffset = 0, bool backFaceCull = true, DrawMode drawMode = DrawMode::FILL)
	{
		s_RendererAPI->DrawIndexed(indexCount, startIndex, vertexOffset, backFaceCull, drawMode);
	}

	inline static void DrawLines(uint32_t vertexCount = 0)
	{
		s_RendererAPI->DrawLines(vertexCount);
	}
private:
	static Scope<RendererAPI> s_RendererAPI;
};