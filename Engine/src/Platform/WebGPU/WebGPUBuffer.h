#pragma once

#include "Renderer/Buffer.h"
#include "WebGPUContext.h"

#include <webgpu/webgpu.hpp>

class WebGPUVertexBuffer : public VertexBuffer
{
public:
	WebGPUVertexBuffer(uint32_t size);
	WebGPUVertexBuffer(void* vertices, uint32_t size);
	~WebGPUVertexBuffer();

	virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout;}
	virtual const BufferLayout& GetLayout() const override { return m_Layout; }

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	virtual void SetData(const void* data) override;

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetSize() override { return m_Size; }
private:
	wgpu::Buffer m_Buffer;
	BufferLayout m_Layout;
	uint32_t m_Size;

	Ref<WebGPUContext> m_WebGPUContext;
};

class WebGPUIndexBuffer : public IndexBuffer
{
public:
	WebGPUIndexBuffer(uint32_t* indices, uint32_t count);
	~WebGPUIndexBuffer();
	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetCount() const override { return m_Count; }
private:
	wgpu::Buffer m_Buffer;
	uint32_t m_Count;

	Ref<WebGPUContext> m_WebGPUContext;
};