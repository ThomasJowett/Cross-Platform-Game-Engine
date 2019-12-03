#pragma once

#include "RendererAPI.h"

class RenderCommand
{
public:
	inline static void SetClearColour(float red, float green, float blue, float alpha)
	{
		s_RendererAPI->SetClearColour(red, green, blue, alpha);
	}

	inline static void Clear()
	{
		s_RendererAPI->Clear();
	}

	inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		s_RendererAPI->DrawIndexed(vertexArray);
	}
private:
	static RendererAPI* s_RendererAPI;
};