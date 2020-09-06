#pragma once

#include "RendererAPI.h"


class RenderCommand
{
public:
	static void CreateRendererAPI();

	inline static bool Init()
	{
		return s_RendererAPI->Init();
	}
	/// <summary>
	/// Sets the clear colour of the screen or currently bound frame buffer
	/// </summary>
	/// <param name="colour"></param>
	inline static void SetClearColour(const Colour& colour)
	{
		s_RendererAPI->SetClearColour(colour);
	}
	/// <summary>
	/// Sets the area of the screen that will be rendered to
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		return s_RendererAPI->SetViewport(x, y, width, height);
	}
	/// <summary>
	/// Resets the pixels of the screen back to the clear colour
	/// </summary>
	inline static void Clear()
	{
		s_RendererAPI->Clear();
	}
	/// <summary>
	/// Draws primitives from the vertex array
	/// </summary>
	/// <param name="vertexArray"></param>
	/// <param name="indexCount"></param>
	/// <param name="drawMode"></param>
	inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::FILL)
	{
		s_RendererAPI->DrawIndexed(vertexArray, indexCount, drawMode);
	}
private:
	static Scope<RendererAPI> s_RendererAPI;
};