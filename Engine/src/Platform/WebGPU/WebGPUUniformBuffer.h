#pragma once

#include "Renderer/UniformBuffer.h"
#include "WebGPUContext.h"
#include <webgpu/webgpu.hpp>

class WebGPUUniformBuffer : public UniformBuffer
{
public:
	WebGPUUniformBuffer(uint32_t size, uint32_t binding);
	virtual ~WebGPUUniformBuffer();

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
private:
	wgpu::BufferDescriptor m_BufferDesc;
	wgpu::Buffer m_UniformBuffer;

	Ref<WebGPUContext> m_WebGPUContext;
};