#include "stdafx.h"
#include "WebGPUContext.h"
#include "Logging/Instrumentor.h"
#include <glad/glad.h>

#include <glfw3webgpu.h>

WebGPUContext::WebGPUContext(GLFWwindow* windowHandle)
	:m_WindowHandle(windowHandle)
{
	CORE_ASSERT(windowHandle, "Window Handle is null")
}

void WebGPUContext::Init()
{
	PROFILE_FUNCTION();
	glfwMakeContextCurrent(m_WindowHandle);

	m_Instance = wgpuCreateInstance(nullptr);
	m_Surface = glfwCreateWindowWGPUSurface(m_Instance, m_WindowHandle);

	wgpu::RequestAdapterOptions adapterOpts{};
	adapterOpts.compatibleSurface = m_Surface;

	m_Adapter = m_Instance.requestAdapter(adapterOpts);

	wgpu::AdapterProperties properties = {};
	m_Adapter.getProperties(&properties);

	ENGINE_INFO("Graphics Card: {0}", properties.name);

	wgpu::DeviceDescriptor deviceDesc = {};
	deviceDesc.label = "Main Device";
	deviceDesc.requiredFeatureCount = 0;
	deviceDesc.nextInChain = nullptr;
	deviceDesc.defaultQueue.label = "The Default queue";
	deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /*pUserData*/)
	{
		ENGINE_CRITICAL("Device lost: Reason: {0} Message: {1}", (int)reason, message);
	};

	m_Device = m_Adapter.requestDevice(deviceDesc);

	auto onDeviceError = [](wgpu::ErrorType type, char const* message)
	{
		ENGINE_ERROR("Uncaptured device error: type {0}", (int)type);
		if (message)
			ENGINE_ERROR(message);
	};

	m_ErrorCallbackHandle = m_Device.setUncapturedErrorCallback(std::move(onDeviceError));
	m_Queue = m_Device.getQueue();

	int width, height;
	glfwGetWindowSize(m_WindowHandle, &width, &height);
	m_SurfaceConfig.width = width;
	m_SurfaceConfig.height = height;
	m_SurfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
	wgpu::TextureFormat surfaceFormat = m_Surface.getPreferredFormat(m_Adapter);
	m_SurfaceConfig.format = surfaceFormat;

	m_SurfaceConfig.viewFormatCount = 0;
	m_SurfaceConfig.viewFormats = nullptr;
	m_SurfaceConfig.device = m_Device;
	m_SurfaceConfig.presentMode = wgpu::PresentMode::Fifo;
	m_SurfaceConfig.alphaMode = wgpu::CompositeAlphaMode::Auto;

	m_Surface.configure(m_SurfaceConfig);
}

void WebGPUContext::SwapBuffers()
{
	PROFILE_FUNCTION();
	m_Surface.present();
#if defined(WEBGPU_BACKEND_DAWN)
	m_Device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
	m_Device.poll(false);
#endif
}

void WebGPUContext::SetSwapInterval(uint32_t interval)
{
	m_SurfaceConfig.presentMode = interval == 1 ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Immediate;
	m_Surface.configure(m_SurfaceConfig);
}

void WebGPUContext::MakeCurrent()
{
}

wgpu::Device WebGPUContext::GetWebGPUDevice()
{
	return m_Device;
}

wgpu::TextureFormat WebGPUContext::GetSwapchainFormat()
{
	return m_Surface.getPreferredFormat(m_Adapter);
}

wgpu::Queue WebGPUContext::GetQueue()
{
	return m_Queue;
}
