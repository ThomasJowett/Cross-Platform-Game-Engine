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
	virtual void Bind() const override;
private:
	uint32_t m_Binding;
	wgpu::BufferDescriptor m_BufferDesc;
	wgpu::Buffer m_UniformBuffer;

	Ref<WebGPUContext> m_WebGPUContext;
};