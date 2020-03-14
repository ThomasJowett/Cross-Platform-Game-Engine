#include "stdafx.h"
#ifdef __WINDOWS__
#include "DirectX11Context.h"

DirectX11Context::DirectX11Context()
{
}

DirectX11Context::~DirectX11Context()
{
	if (m_SwapChain) m_SwapChain->Release();
}

void DirectX11Context::Init()
{
}

void DirectX11Context::SwapBuffers()
{
	m_SwapChain->Present(0, 0);
}
#endif