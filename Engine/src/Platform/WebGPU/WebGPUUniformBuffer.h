#pragma once

#include "Renderer/UniformBuffer.h"

class WebGPUUniformBuffer : public UniformBuffer
{
public:
	WebGPUUniformBuffer(uint32_t size, uint32_t binding);
	virtual ~WebGPUUniformBuffer();

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
private:
	uint32_t m_RendererID = 0;
};