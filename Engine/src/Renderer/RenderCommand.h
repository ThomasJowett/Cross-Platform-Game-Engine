#pragma once

#include "RendererAPI.h"


class RenderCommand
{
public:
	/**
	 * Create a Renderer API object
	 */
	static void CreateRendererAPI();

	/**
	 * Initialise the Renderer
	 * @return true Renderer created successfully
	 * @return false Renderer was not created
	 */
	inline static bool Init()
	{
		return s_RendererAPI->Init();
	}

	/**
	 * Sets the clear colour of the screen or currently bound frame buffer
	 * @param colour 
	 */
	inline static void SetClearColour(const Colour& colour)
	{
		s_RendererAPI->SetClearColour(colour);
	}

	/**
	 * Sets the area of the screen that will be rendered to
	 * @param x 
	 * @param y 
	 * @param width 
	 * @param height 
	 */
	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		return s_RendererAPI->SetViewport(x, y, width, height);
	}

	/**
	 * Resets the pixels of the screen back to the clear colour
	 */
	inline static void Clear()
	{
		s_RendererAPI->Clear();
	}
	
	/**
	 * Draws primitives from the vertex array
	 * @param vertexArray 
	 * @param indexCount 
	 * @param drawMode 
	 */
	inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::FILL)
	{
		s_RendererAPI->DrawIndexed(vertexArray, indexCount, drawMode);
	}
private:
	static Scope<RendererAPI> s_RendererAPI;
};