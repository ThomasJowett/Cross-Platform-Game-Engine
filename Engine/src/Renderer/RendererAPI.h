#pragma once

#include "VertexArray.h"
#include "Core/Colour.h"

enum class DrawMode
{
	POINTS,
	WIREFRAME,
	FILL
};

class RendererAPI
{
	friend class RenderCommand;
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
	virtual ~RendererAPI() = default;
	virtual bool Init() = 0;
	virtual void SetClearColour(const Colour& colour) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::FILL) = 0;
	virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount = 0) = 0;

	inline static API GetAPI() { return s_API; }
protected:
		static API s_API;
};
