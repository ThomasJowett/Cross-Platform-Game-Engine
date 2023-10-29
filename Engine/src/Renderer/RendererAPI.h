#pragma once

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
	virtual void ClearColour() = 0;
	virtual void ClearDepth() = 0;

	virtual void DrawIndexed(uint32_t indexCount = 0, uint32_t startIndex = 0, uint32_t vertexOffset = 0, bool backFaceCull = true, DrawMode drawMode = DrawMode::FILL) = 0;
	virtual void DrawLines(uint32_t vertexCount = 0) = 0;

	inline static API GetAPI() { return s_API; }
protected:
	static API s_API;
};
