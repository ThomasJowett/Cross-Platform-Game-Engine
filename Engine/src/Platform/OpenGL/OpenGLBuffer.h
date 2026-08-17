#pragma once

#include "Renderer/Buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
	OpenGLVertexBuffer(uint32_t size);
	OpenGLVertexBuffer(void* vertices, uint32_t size);
	~OpenGLVertexBuffer();

	// Just records the layout this buffer's bytes conform to (used to seed a Pipeline::Spec
	// at pipeline-creation time, e.g. Renderer2D.cpp's quadSpec.layout = quadVertexBuffer->
	// GetLayout()) - actual GL vertex-attribute setup now happens lazily, per the currently
	// bound pipeline's own Spec::layout, via OpenGLPipeline::ConfigureVertexBuffer() (called
	// from Bind() below), not here.
	virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	virtual const BufferLayout& GetLayout() const override { return m_Layout; }

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	virtual void SetData(const void* data) override;

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetSize() override { return m_Size; }
private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
	uint32_t m_Size;
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
	~OpenGLIndexBuffer();
	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetCount() const override { return m_Count; }
private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};