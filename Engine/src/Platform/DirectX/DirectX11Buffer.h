#pragma once
#include "Renderer/Buffer.h"

class DirectX11VertexBuffer :
	public VertexBuffer
{
public:
	DirectX11VertexBuffer(uint32_t size);
	DirectX11VertexBuffer(float* vertices, uint32_t size);
	~DirectX11VertexBuffer();

	virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	virtual const BufferLayout& GetLayout() override { return m_Layout; }

	virtual void SetData(const void* data, uint32_t size) override;

	virtual void Bind() const override;
	virtual void UnBind() const override;
private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
};

class DirectX11IndexBuffer : public IndexBuffer
{
public:
	DirectX11IndexBuffer(uint32_t* indices, uint32_t count);
	~DirectX11IndexBuffer();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetCount() const override { return m_Count; }
private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};