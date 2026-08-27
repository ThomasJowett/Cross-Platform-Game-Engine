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
	virtual void ResizeBuffers(uint32_t width, uint32_t height) override;
	virtual void SetSwapInterval(uint32_t interval) override;
	virtual void MakeCurrent() override;

	wgpu::Device GetWebGPUDevice();
	wgpu::Surface GetSurface();
	wgpu::TextureFormat GetSwapchainFormat();
	wgpu::Queue GetQueue();
	const wgpu::SurfaceConfiguration& GetSurfaceConfig() const { return m_SurfaceConfig; }
	wgpu::TextureView GetCurrentTextureView();

	// Tracks whether anything has already rendered to the swapchain surface this frame, so a
	// later pass (e.g. ImGui's own hand-rolled one) knows to load rather than clear - otherwise
	// whichever of a scene render and an ImGui overlay runs second would erase the other's
	// output instead of compositing over it. Reset once per frame in SwapBuffers().
	bool HasSwapchainBeenRenderedThisFrame() const { return m_SwapchainRenderedThisFrame; }
	void MarkSwapchainRenderedThisFrame() { m_SwapchainRenderedThisFrame = true; }

	void PollEvents();

private:
	void SetupSurface();

	GLFWwindow* m_WindowHandle;
	wgpu::Instance m_Instance;
	wgpu::Surface m_Surface;
	wgpu::Adapter m_Adapter;
	wgpu::Device m_Device;
	wgpu::Queue m_Queue;

	wgpu::SurfaceConfiguration m_SurfaceConfig;

	std::unique_ptr<wgpu::ErrorCallback> m_ErrorCallbackHandle;
	std::unique_ptr<wgpu::RequestAdapterCallback> m_RequestAdapterCallbackHandle;
	std::unique_ptr<wgpu::RequestDeviceCallback> m_RequestDeviceCallbackHandle;

	wgpu::SurfaceTexture m_CurrentSurfaceTexture;
	wgpu::TextureView m_CurrentTextureView;

	bool m_SurfaceAcquired = false;
	bool m_NeedsResize = false;
	bool m_Initialized = false;
	bool m_SwapchainRenderedThisFrame = false;
};