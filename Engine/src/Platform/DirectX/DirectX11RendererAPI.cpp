#include "stdafx.h"
#include "DirectX11RendererAPI.h"
#include "Core/Application.h"
#include "DirectX11Context.h"

#include <d3d11.h>

extern ID3D11Device* g_D3dDevice;
extern ID3D11DeviceContext* g_ImmediateContext;

DirectX11RendererAPI::~DirectX11RendererAPI()
{
	if (m_RSWireFrame) m_RSWireFrame->Release();
	if (m_RSFill) m_RSFill->Release();
	if (m_RSPoints) m_RSPoints->Release();
	if (m_RSCullNone) m_RSCullNone->Release();
}

bool DirectX11RendererAPI::Init()
{
	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	HR(g_D3dDevice->CreateRasterizerState(&cmdesc, &m_RSFill));

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	HR(g_D3dDevice->CreateRasterizerState(&cmdesc, &m_RSPoints));

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_WIREFRAME;
	cmdesc.CullMode = D3D11_CULL_NONE;
	HR(g_D3dDevice->CreateRasterizerState(&cmdesc, &m_RSWireFrame));

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	HR(g_D3dDevice->CreateRasterizerState(&cmdesc, &m_RSCullNone));

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
	g_ImmediateContext->RSSetViewports(1, &m_Viewport);
}

void DirectX11RendererAPI::Clear()
{
	DirectX11Context* context = dynamic_cast<DirectX11Context*>(Application::GetWindow()->GetContext().get());
	auto renderTarget = context->GetRenderTargetView();

	if (renderTarget)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &renderTarget, NULL);
		g_ImmediateContext->ClearRenderTargetView(context->GetRenderTargetView(), &m_ClearColour.r);
		g_ImmediateContext->ClearDepthStencilView(context->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void DirectX11RendererAPI::ClearColour()
{
	DirectX11Context* context = dynamic_cast<DirectX11Context*>(Application::GetWindow()->GetContext().get());
	auto renderTarget = context->GetRenderTargetView();

	if (renderTarget)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &renderTarget, NULL);
		g_ImmediateContext->ClearRenderTargetView(context->GetRenderTargetView(), &m_ClearColour.r);
	}
}

void DirectX11RendererAPI::ClearDepth()
{
	DirectX11Context* context = dynamic_cast<DirectX11Context*>(Application::GetWindow()->GetContext().get());
	auto renderTarget = context->GetRenderTargetView();

	if (renderTarget)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &renderTarget, NULL);
		g_ImmediateContext->ClearDepthStencilView(context->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void DirectX11RendererAPI::DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode)
{
	if (!backFaceCull)
	{
		g_ImmediateContext->RSSetState(m_RSCullNone);
	}
	else
	{
		switch (drawMode)
		{
		case DrawMode::POINTS:
			g_ImmediateContext->RSSetState(m_RSPoints);
			break;
		case DrawMode::WIREFRAME:
			g_ImmediateContext->RSSetState(m_RSWireFrame);
			break;
		case DrawMode::FILL:
			g_ImmediateContext->RSSetState(m_RSFill);
			break;
		default:
			break;
		}
	}

	g_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_ImmediateContext->DrawIndexed(indexCount, startIndex, vertexOffset);
}

void DirectX11RendererAPI::DrawLines(uint32_t vertexCount)
{
}