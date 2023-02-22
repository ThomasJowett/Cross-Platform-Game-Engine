#pragma once
#include "Renderer/Buffer.h"

class DirectX11VertexBuffer :
	public VertexBuffer
{
public:
	DirectX11VertexBuffer(uint32_t size);
	DirectX11VertexBuffer(void* vertices, uint32_t size);
	~DirectX11VertexBuffer();

	virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	virtual const BufferLayout& GetLayout() const override { return m_Layout; }

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	virtual void SetData(const void* data) override;

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual uint32_t GetSize() { return m_Size; }
private:
	BufferLayout m_Layout;

	ID3D11Buffer* m_VertexBuffer;

	UINT m_Offset;
	UINT m_Size;
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
	ID3D11Buffer* m_IndexBuffer;
	uint32_t m_Count;
};