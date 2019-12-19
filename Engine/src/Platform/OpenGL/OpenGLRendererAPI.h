#pragma once

#include "Renderer/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
	virtual bool Init() override;
	virtual void SetClearColour(float red, float green, float blue, float alpha) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
};