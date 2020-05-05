#include "stdafx.h"
#include "DirectX11RendererAPI.h"
#include "Core/Application.h"
#include "Platform/Windows/Win32Window.h"
#include "DirectX11Context.h"

#include <d3d11.h>

bool DirectX11RendererAPI::Init()
{
	DirectX11Context* context = dynamic_cast<DirectX11Context*>(Application::GetWindow().GetContext().get());
	m_ImmediateContext = context->GetDeviceContext();
	m_RenderTargetView = context->GetRenderTargetView();
	m_DepthStencilView = context->GetDepthStencilView();
	return true;
}

void DirectX11RendererAPI::SetClearColour(const Colour& colour)
{
	m_ClearColour = colour;
}

void DirectX11RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	m_Viewport.Width = (FLOAT)width;
	m_Viewport.Height = (FLOAT)height;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.TopLeftX = (FLOAT)x;
	m_Viewport.TopLeftY = (FLOAT)y;
	m_ImmediateContext->RSSetViewports(1, &m_Viewport);
}

void DirectX11RendererAPI::Clear()
{
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView, &m_ClearColour.r);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DirectX11RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, DrawMode drawMode)
{
	vertexArray->Bind();

	switch (drawMode)
	{
	case DrawMode::POINTS:
		m_ImmediateContext->RSSetState(m_RSPoints);
		break;
	case DrawMode::WIREFRAME:
		m_ImmediateContext->RSSetState(m_RSWireFrame);
		break;
	case DrawMode::FILL:
		m_ImmediateContext->RSSetState(m_RSFill);
		break;
	default:
		break;
	}

	m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	m_ImmediateContext->DrawIndexed(count, 0, 0);
}