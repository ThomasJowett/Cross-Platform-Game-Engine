#pragma once

#include "Core/Colour.h"

class RendererAPI
{
	friend class RenderCommand;
public:
	enum class API
	{
		None = 0,
		OpenGL = 1,
		WebGPU
	};

public:
	virtual ~RendererAPI() = default;
	virtual bool Init() = 0;
	virtual void SetClearColour(const Colour& colour) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void Clear() = 0;
	virtual void ClearColour() = 0;
	virtual void ClearDepth() = 0;

	// clear=false loads the render pass's existing attachment contents instead of clearing them -
	// needed for passes that draw on top of an already-rendered framebuffer (e.g. editor gizmo overlays).
	virtual void StartRenderPass(bool clear = true) = 0;
	virtual void EndRenderPass() = 0;

	virtual void DrawIndexed(uint32_t indexCount = 0, uint32_t startIndex = 0, uint32_t vertexOffset = 0) = 0;
	virtual void DrawLines(uint32_t vertexCount = 0) = 0;

	inline static API GetAPI() { return s_API; }

	inline static const char* GetAPIName()
	{
		switch (s_API)
		{
		case API::OpenGL: return "OpenGL";
		case API::WebGPU: return "WebGPU";
		default:          return "None";
		}
	}
protected:
	static API s_API;
};
