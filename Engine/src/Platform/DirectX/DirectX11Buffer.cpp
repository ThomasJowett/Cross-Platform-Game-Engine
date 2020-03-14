#include "stdafx.h"
#ifdef __WINDOWS__
#include "DirectX11Buffer.h"

DirectX11IndexBuffer::DirectX11IndexBuffer(uint32_t * indices, uint32_t count)
{
}

DirectX11IndexBuffer::~DirectX11IndexBuffer()
{
}

void DirectX11IndexBuffer::Bind() const
{
}

void DirectX11IndexBuffer::UnBind() const
{
}

DirectX11VertexBuffer::DirectX11VertexBuffer(float * vertices, uint32_t size)
{
}

DirectX11VertexBuffer::~DirectX11VertexBuffer()
{
}

void DirectX11VertexBuffer::Bind() const
{
}

void DirectX11VertexBuffer::UnBind() const
{
}
#endif