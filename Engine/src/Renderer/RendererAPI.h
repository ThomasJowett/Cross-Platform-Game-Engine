#pragma once

#include "VertexArray.h"

class RendererAPI
{
public:
	enum class API
	{
		None = 0,
		OpenGL = 1,
		Directx11,
		Metal,
		Vulkan
	};

public:
	virtual bool Init() = 0;
	virtual void SetClearColour(float red, float green, float blue, float alpha) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

	inline static API GetAPI() { return s_API; }
private:
		static API s_API;
};
