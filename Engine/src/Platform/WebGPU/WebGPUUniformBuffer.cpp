#include "stdafx.h"
#include "WebGPUUniformBuffer.h"

#include "Core/Application.h"

WebGPUUniformBuffer::WebGPUUniformBuffer(uint32_t size, uint32_t binding)
{
	m_BufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
	m_BufferDesc.size = size;
	m_BufferDesc.mappedAtCreation = false;

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	auto device = m_WebGPUContext->GetWebGPUDevice();

	m_UniformBuffer = device.createBuffer(m_BufferDesc);

}

WebGPUUniformBuffer::~WebGPUUniformBuffer()
{
	if (Application::Get().IsRunning() && m_UniformBuffer)
		m_UniformBuffer.release();
	m_UniformBuffer = nullptr;
}

void WebGPUUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	m_WebGPUContext->GetQueue().writeBuffer(m_UniformBuffer, offset, data, size);
}
