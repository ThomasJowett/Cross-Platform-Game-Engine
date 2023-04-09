#pragma once

#include "Renderer/Buffer.h"

class VulkanVertexBuffer : public VertexBuffer
{
public:
	VulkanVertexBuffer(uint32_t size);
	VulkanVertexBuffer(void* vertices, uint32_t size);
	~VulkanVertexBuffer();

	// Inherited via VertexBuffer
	virtual void SetLayout(const BufferLayout& layout) override;
	virtual const BufferLayout& GetLayout() const override;
	virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;
	virtual void SetData(const void* data) override;
	virtual uint32_t GetSize() override;
	virtual void Bind() const override;
	virtual void UnBind() const override;
private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
	VulkanIndexBuffer(uint32_t* indices, uint32_t count);
	~VulkanIndexBuffer();

	// Inherited via IndexBuffer
	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual uint32_t GetCount() const override;

private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};