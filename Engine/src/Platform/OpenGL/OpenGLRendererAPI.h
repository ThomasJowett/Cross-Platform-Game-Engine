#pragma once

#include "Renderer/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
	virtual void SetClearColour(float red, float green, float blue, float alpha) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
};