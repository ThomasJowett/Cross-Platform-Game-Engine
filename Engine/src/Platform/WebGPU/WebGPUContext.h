#pragma once

#include "Core/core.h"

#include "Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

#include <webgpu/webgpu.hpp>

class WebGPUContext : public GraphicsContext
{
public:
	WebGPUContext(GLFWwindow* windowHandle);
	virtual void Init() override;
	virtual void SwapBuffers() override;
	virtual void ResizeBuffers(uint32_t width, uint32_t height) override {};
	virtual void SetSwapInterval(uint32_t interval) override;
	virtual void MakeCurrent() override;

	wgpu::Device GetWebGPUDevice();
	wgpu::TextureFormat GetSwapchainFormat();
	wgpu::Queue GetQueue();
    
private:
	GLFWwindow* m_WindowHandle;
	wgpu::Instance m_Instance;
	wgpu::Surface m_Surface;
	wgpu::Adapter m_Adapter;
	wgpu::Device m_Device;
	wgpu::Queue m_Queue;

	wgpu::SurfaceConfiguration m_SurfaceConfig;

	std::unique_ptr<wgpu::ErrorCallback> m_ErrorCallbackHandle;
};