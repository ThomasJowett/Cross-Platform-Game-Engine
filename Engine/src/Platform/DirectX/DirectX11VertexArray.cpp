#include "stdafx.h"
#include "DirectX11VertexArray.h"

DirectX11VertexArray::DirectX11VertexArray()
{
}

DirectX11VertexArray::~DirectX11VertexArray()
{
}

void DirectX11VertexArray::Bind() const
{
}

void DirectX11VertexArray::UnBind() const
{
}

void DirectX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	// TODO: add index buffer directX11
	vertexBuffer->Bind();

	CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout has no elements");

	m_VertexBuffers.push_back(vertexBuffer);
}

void DirectX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	// TODO: set index buffer directX11
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

const std::vector<Ref<VertexBuffer>>& DirectX11VertexArray::GetVertexBuffers() const
{
	return m_VertexBuffers;
}

const Ref<IndexBuffer>& DirectX11VertexArray::GetIndexBuffer() const
{
	return m_IndexBuffer;
}