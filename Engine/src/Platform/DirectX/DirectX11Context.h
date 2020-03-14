#pragma once
#ifdef __WINDOWS__
#include <d3d11_1.h>

#include "Core/core.h"

#include "Renderer/GraphicsContext.h"

class DirectX11Context : public GraphicsContext
{
public:
	DirectX11Context();
	~DirectX11Context();
	virtual void Init()override;
	virtual void SwapBuffers()override;
private:
	HWND m_WindowHandle;

	IDXGISwapChain* m_SwapChain;
};
#endif