#pragma once

#include "Renderer/RendererAPI.h"

class VulkanRendererAPI : public RendererAPI
{
public:
	virtual ~VulkanRendererAPI();

	// Inherited via RendererAPI
	virtual bool Init() override;
	virtual void SetClearColour(const Colour& colour) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void Clear() override;
	virtual void ClearColour()override;
	virtual void ClearDepth()override;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode) override;
	virtual void DrawLines(uint32_t vertexCount) override;
private:
	Colour m_ClearColour;
};
