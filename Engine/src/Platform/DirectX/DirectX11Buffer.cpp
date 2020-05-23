#include "stdafx.h"
#include "DirectX11Buffer.h"

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

DirectX11IndexBuffer::DirectX11IndexBuffer(uint32_t* indices, uint32_t count)
	:m_Count(count)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = count * sizeof(uint32_t);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &indices[0];

	g_D3dDevice->CreateBuffer(&bd, &initData, &m_IndexBuffer);
}

DirectX11IndexBuffer::~DirectX11IndexBuffer()
{
	if (m_IndexBuffer) m_IndexBuffer->Release();
}

void DirectX11IndexBuffer::Bind() const
{
	g_ImmediateContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
}

void DirectX11IndexBuffer::UnBind() const
{
	g_ImmediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
}

//------------------------------------------------------------------------------------

DirectX11VertexBuffer::DirectX11VertexBuffer(uint32_t size)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_D3dDevice->CreateBuffer(&bd, nullptr, &m_VertexBuffer);

	m_Offset = 0;
}

DirectX11VertexBuffer::DirectX11VertexBuffer(float* vertices, uint32_t size)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &vertices[0];

	g_D3dDevice->CreateBuffer(&bd, &initData, &m_VertexBuffer);

	m_Offset = 0;
}

DirectX11VertexBuffer::~DirectX11VertexBuffer()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
}

void DirectX11VertexBuffer::SetData(const void* data, uint32_t size)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	g_ImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	g_ImmediateContext->Unmap(m_VertexBuffer, 0);
}

void DirectX11VertexBuffer::Bind() const
{
	const UINT stride = m_Layout.GetStride();
	g_ImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &m_Offset);
}

void DirectX11VertexBuffer::UnBind() const
{
	g_ImmediateContext->IASetVertexBuffers(0, 1, nullptr, 0, 0);
}