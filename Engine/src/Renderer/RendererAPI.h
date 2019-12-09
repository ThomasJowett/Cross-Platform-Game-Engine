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
	virtual void SetClearColour(float red, float green, float blue, float alpha) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

	inline static API GetAPI() { return s_API; }
private:
		static API s_API;
};
