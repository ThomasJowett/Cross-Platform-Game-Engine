#include "stdafx.h"
#include "DirectX11Context.h"
#include "DirectX11RendererAPI.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Core/Settings.h"

#pragma comment(lib, "d3d11.lib") 

extern ID3D11Device* g_D3dDevice = nullptr;
extern ID3D11DeviceContext* g_ImmediateContext = nullptr;

DirectX11Context::DirectX11Context(HWND windowHandle)
	:m_WindowHandle(windowHandle)
{
	m_SwapChain = nullptr;

	m_RenderTargetView = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_DepthStencilView = nullptr;

	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	CORE_ASSERT(windowHandle, "Window Handle is null")
}

DirectX11Context::~DirectX11Context()
{
	if (m_RenderTargetView) m_RenderTargetView->Release();
	if (m_DepthStencilView) m_DepthStencilView->Release();

	if (m_SwapChain) m_SwapChain->Release();
	if (g_D3dDevice) g_D3dDevice->Release();
	if (g_ImmediateContext) g_ImmediateContext->Release();
}

void DirectX11Context::Init()
{
	int renderWidth = Settings::GetInt("Display", "Screen_Width");
	int renderHeight = Settings::GetInt("Display", "Screen_Height");

	m_SyncInterval = (Settings::GetBool("Display", "V-Sync")) ? 1 : 0;

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

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

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
	sd.BufferDesc.Width = renderWidth;
	sd.BufferDesc.Height = renderHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_WindowHandle;
	sd.SampleDesc.Count = sampleCount;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_DriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, m_DriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_SwapChain, &g_D3dDevice, &m_FeatureLevel, &g_ImmediateContext);
	
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		CORE_ASSERT(FAILED(hr), "Failed to create swap chain");
		ENGINE_CRITICAL("Failed to create Device and swap chain: {0}", hr);
	}

	// Create the render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

	if (FAILED(hr))
	{
		ENGINE_CRITICAL("Failed to retrieve back buffer from swap chain: {0}", hr);
	}

	hr = g_D3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_RenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
	{
		ENGINE_CRITICAL("Failed to create the render target view: {0}", hr);
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = renderWidth;
	depthStencilDesc.Height = renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = g_D3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_DepthStencilBuffer);
	if (FAILED(hr))
	{
		ENGINE_CRITICAL("Failed to create depth stencil buffer: {0}", hr);
	}

	hr = g_D3dDevice->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, &m_DepthStencilView);
	if (FAILED(hr))
	{
		ENGINE_CRITICAL("Failed to create depth stencil view: {0}", hr);
	}
}

void DirectX11Context::SwapBuffers()
{
	m_SwapChain->Present(m_SyncInterval, 0);
}

void DirectX11Context::SetSwapInterval(uint32_t interval) 
{
	m_SyncInterval = (UINT)interval;
}
