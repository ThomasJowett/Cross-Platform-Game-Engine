#pragma once

#include "Renderer/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
	virtual bool Init() override;
	virtual void SetClearColour(const Colour& colour) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, DrawMode drawMode = DrawMode::FILL) override;
	virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
};