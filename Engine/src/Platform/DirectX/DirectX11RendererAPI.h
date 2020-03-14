#pragma once

#include "Renderer/RendererAPI.h"

#include <d3d11.h>

class DirectX11RendererAPI : public RendererAPI
{
	virtual bool Init() override;
	virtual void SetClearColour(const Colour& colour) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void Clear() override;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, DrawMode drawMode = DrawMode::FILL) override;

private:
	D3D11_VIEWPORT m_Viewport;
	ID3D11DeviceContext* m_ImmediateContext;

	Colour m_ClearColour;
};