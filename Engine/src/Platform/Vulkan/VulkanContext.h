#pragma once

#include "Renderer/GraphicsContext.h"

class VulkanContext : public GraphicsContext
{
public:
	VulkanContext();


	// Inherited via GraphicsContext
	virtual void Init() override;

	virtual void SwapBuffers() override;

	virtual void ResizeBuffers(uint32_t width, uint32_t height) override;

	virtual void SetSwapInterval(uint32_t interval) override;

	virtual void MakeCurrent() override;

private:

};
