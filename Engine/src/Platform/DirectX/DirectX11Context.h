#pragma once
#include <d3d11_1.h>

#include "Core/core.h"

#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

class DirectX11Context : public GraphicsContext
{
public:
	DirectX11Context(HWND windowHandle);
	~DirectX11Context();
	virtual void Init()override;
	virtual void SwapBuffers()override;

	ID3D11Device* GetDevice() { return m_D3dDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return m_ImmediateContext; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }
private:
	HWND m_WindowHandle;
	IDXGISwapChain* m_SwapChain;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	ID3D11Texture2D* m_DepthStencilBuffer;

	ID3D11DeviceContext* m_ImmediateContext;
	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;

	ID3D11Device* m_D3dDevice;
};