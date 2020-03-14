#include "stdafx.h"
#ifdef __WINDOWS__
#include "DirectX11RendererAPI.h"

bool DirectX11RendererAPI::Init()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 1;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	//sd.BufferDesc.Width = _render
	//sd.BufferDesc.Height = _renderHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//sd.OutputWindow = m_WindowHandle;
	sd.SampleDesc.Count = sampleCount;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	return false;
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
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
}

void DirectX11RendererAPI::Clear()
{
	//m_ImmediateContext->ClearRenderTargetView()
	//m_ImmediateContext->ClearDepthStencilView()
}

void DirectX11RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, DrawMode drawMode)
{
	m_ImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ImmediateContext->DrawIndexed(vertexArray->GetIndexBuffer()->GetCount(), 0, 0);
}
#endif