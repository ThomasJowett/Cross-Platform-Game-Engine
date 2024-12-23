#include "stdafx.h"
#include "WebGPUBuffer.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "Renderer/RenderCommand.h"

WebGPUVertexBuffer::WebGPUVertexBuffer(uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
	wgpu::BufferDescriptor bufferDesc{};
	bufferDesc.size = size;
	bufferDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
	bufferDesc.mappedAtCreation = false;

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	auto device = m_WebGPUContext->GetWebGPUDevice();
	m_Buffer = device.createBuffer(bufferDesc);
}

WebGPUVertexBuffer::WebGPUVertexBuffer(void* vertices, uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
	wgpu::BufferDescriptor bufferDesc{};
	bufferDesc.size = size;
	bufferDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
	bufferDesc.mappedAtCreation = true;
	
	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	auto device = m_WebGPUContext->GetWebGPUDevice();
	m_Buffer = device.createBuffer(bufferDesc);

	void* mappedData = m_Buffer.getMappedRange(0, size);
	memcpy(mappedData, vertices, size);
	m_Buffer.unmap();
}

WebGPUVertexBuffer::~WebGPUVertexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		m_Buffer.release();
}

void WebGPUVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	CORE_ASSERT(size + offset <= m_Size, "Size must be less than the buffer size");

	PROFILE_FUNCTION();
	void* mappedMemory = m_Buffer.getMappedRange(offset, size);
	CORE_ASSERT(mappedMemory, "Failed to map buffer for writing");

	memcpy(mappedMemory, data, size);
	m_Buffer.unmap();
}

void WebGPUVertexBuffer::SetData(const void* data)
{
	SetData(data, m_Size);
}

void WebGPUVertexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	//TODO: get the renderpass and call 
	// renderPass.setVertexBuffer(0, m_Buffer, 0, m_Buffer.getSize());
}

void WebGPUVertexBuffer::UnBind() const
{
}

//------------------------------------------------------------------------------------

WebGPUIndexBuffer::WebGPUIndexBuffer(uint32_t* indices, uint32_t count)
	:m_Count(count)
{
	PROFILE_FUNCTION();
	uint32_t size = count * sizeof(uint32_t);

	wgpu::BufferDescriptor bufferDesc{};
	bufferDesc.size = size;
	bufferDesc.usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
	bufferDesc.mappedAtCreation = true;

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	auto device = m_WebGPUContext->GetWebGPUDevice();

	m_Buffer = device.createBuffer(bufferDesc);

	void* mappedData = m_Buffer.getMappedRange(0, size);
	memcpy(mappedData, indices, size);
	m_Buffer.unmap();
}

WebGPUIndexBuffer::~WebGPUIndexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		m_Buffer.release();
}

void WebGPUIndexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	//TODO: get the render pass and call:
	//renderPass.setIndexBuffer(m_Buffer, wgpu::IndexFormat::Uint16, 0, m_Count * sizeof(uint16_t));
}

void WebGPUIndexBuffer::UnBind() const
{
}