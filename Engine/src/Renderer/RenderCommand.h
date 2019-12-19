#pragma once

#include "RendererAPI.h"

class RenderCommand
{
public:
	inline static bool Init()
	{
		return s_RendererAPI->Init();
	}
	inline static void SetClearColour(float red, float green, float blue, float alpha)
	{
		s_RendererAPI->SetClearColour(red, green, blue, alpha);
	}
	inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		return s_RendererAPI->SetViewport(x, y, width, height);
	}

	inline static void Clear()
	{
		s_RendererAPI->Clear();
	}

	inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		s_RendererAPI->DrawIndexed(vertexArray);
	}
private:
	static Scope<RendererAPI> s_RendererAPI;
};