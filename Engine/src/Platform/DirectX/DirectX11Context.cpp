#include "stdafx.h"
#include "DirectX11Context.h"
#include "DirectX11RendererAPI.h"
#include "Core/Application.h"

DirectX11Context::DirectX11Context(HWND windowHandle)
	:m_WindowHandle(windowHandle)
{
	m_SwapChain = nullptr;
	m_ImmediateContext = nullptr;
	m_D3dDevice = nullptr;

	m_DriverType = D3D_DRIVER_TYPE_NULL;
	m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	CORE_ASSERT(windowHandle, "Window Handle is null")
}

DirectX11Context::~DirectX11Context()
{
	if (m_SwapChain) m_SwapChain->Release();
}

void DirectX11Context::Init()
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
	sd.BufferDesc.Width = 1920;
	sd.BufferDesc.Height = 1080;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_WindowHandle;
	sd.SampleDesc.Count = sampleCount;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;


	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_DriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, m_DriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_D3dDevice, &m_FeatureLevel, &m_ImmediateContext);
	
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		ENGINE_CRITICAL("Failed to create Device and swap chain: {0}", hr);
}

void DirectX11Context::SwapBuffers()
{
	m_SwapChain->Present(0, 0);
}