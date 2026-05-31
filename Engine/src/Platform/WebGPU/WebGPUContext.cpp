#include "WebGPUContext.h"
#include "GLFW/glfw3.h"
#include "Logging/Instrumentor.h"
#include <glad/glad.h>

#include <glfw3webgpu.h>
#include <webgpu/webgpu.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

WebGPUContext::WebGPUContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
{
	CORE_ASSERT(windowHandle, "Window Handle is null");
	m_SurfaceConfig.presentMode = wgpu::PresentMode::Fifo;
}

void WebGPUContext::Init()
{
	PROFILE_FUNCTION();

	m_Instance = wgpuCreateInstance(nullptr);
	m_Surface = glfwCreateWindowWGPUSurface(m_Instance, m_WindowHandle);

	wgpu::RequestAdapterOptions adapterOpts{};
	adapterOpts.compatibleSurface = m_Surface;

#ifdef __EMSCRIPTEN__
	m_Instance.requestAdapter(adapterOpts,
	                          [this](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, char const* message)
	                          {
		                          if (status != wgpu::RequestAdapterStatus::Success)
		                          {
			                          ENGINE_CRITICAL("Failed to request WebGPU adapter: {0}", message);
			                          m_Initialized = true;
			                          return;
		                          }

		                          m_Adapter = adapter;

		                          wgpu::AdapterProperties properties = {};
		                          m_Adapter.getProperties(&properties);

		                          wgpu::DeviceDescriptor deviceDesc = {};
		                          deviceDesc.label = "Main Device";
		                          deviceDesc.requiredFeatureCount = 0;
		                          deviceDesc.nextInChain = nullptr;
		                          deviceDesc.defaultQueue.label = "The Default queue";
		                          deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /*pUserData*/)
		                          { ENGINE_CRITICAL("Device lost: Reason: {0} Message: {1}", (int)reason, message); };

		                          m_Adapter.requestDevice(deviceDesc,
		                                                  [this](wgpu::RequestDeviceStatus status, wgpu::Device device, char const* message)
		                                                  {
			                                                  if (status != wgpu::RequestDeviceStatus::Success)
			                                                  {
				                                                  ENGINE_ERROR("Could not create WebGPU device: {0}", message ? message : "Unknown error");
				                                                  m_Initialized = true;
				                                                  return;
			                                                  }

			                                                  m_Device = device;

			                                                  auto OnDeviceError = [](wgpu::ErrorType type, char const* message)
			                                                  {
				                                                  ENGINE_ERROR("Uncaptured device error: type {0}", (int)type);
				                                                  if (message)
					                                                  ENGINE_ERROR(message);
			                                                  };

			                                                  m_ErrorCallbackHandle = m_Device.setUncapturedErrorCallback(std::move(OnDeviceError));
			                                                  m_Queue = m_Device.getQueue();

			                                                  SetupSurface();
			                                                  m_Initialized = true;
		                                                  });
	                          });
	while (!m_Initialized)
	{
		emscripten_sleep(10);
	}
#else
	m_Adapter = m_Instance.requestAdapter(adapterOpts);

	wgpu::AdapterProperties properties = {};
	m_Adapter.getProperties(&properties);

	std::string adapterName = properties.name ? properties.name : "Unknown Adapter";
	ENGINE_INFO("Graphics Card: {0}", adapterName);

	wgpu::DeviceDescriptor deviceDesc = {};
	deviceDesc.label = "Main Device";
	deviceDesc.requiredFeatureCount = 0;
	deviceDesc.nextInChain = nullptr;
	deviceDesc.defaultQueue.label = "The Default queue";
	deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /*pUserData*/) { ENGINE_CRITICAL("Device lost: Reason: {0} Message: {1}", (int)reason, message); };

	m_Device = m_Adapter.requestDevice(deviceDesc);

	auto onDeviceError = [](wgpu::ErrorType type, char const* message)
	{
		ENGINE_ERROR("Uncaptured device error: type {0}", (int)type);
		if (message)
			ENGINE_ERROR(message);
	};

	m_ErrorCallbackHandle = m_Device.setUncapturedErrorCallback(std::move(onDeviceError));
	m_Queue = m_Device.getQueue();

	SetupSurface();
#endif
}

void WebGPUContext::SwapBuffers()
{
	PROFILE_FUNCTION();
	if (m_SurfaceAcquired)
	{
		if (m_CurrentTextureView)
		{
			m_CurrentTextureView.release();
			m_CurrentTextureView = nullptr;
		}
#ifndef __EMSCRIPTEN__
		m_Surface.present();
#endif
		m_SurfaceAcquired = false;
	}
	if (m_NeedsResize && m_Device)
	{
		m_Surface.configure(m_SurfaceConfig);
		m_NeedsResize = false;
	}

	PollEvents();
}

void WebGPUContext::ResizeBuffers(uint32_t width, uint32_t height)
{
	int32_t fbWidth, fbHeight;
	glfwGetFramebufferSize(m_WindowHandle, &fbWidth, &fbHeight);
	if (fbWidth > 0 && fbHeight > 0)
	{
		m_SurfaceConfig.width = fbWidth;
		m_SurfaceConfig.height = fbHeight;
		m_NeedsResize = true;
	}
}

void WebGPUContext::SetSwapInterval(uint32_t interval)
{
	m_SurfaceConfig.presentMode = interval == 1 ? wgpu::PresentMode::Fifo : wgpu::PresentMode::Immediate;
	if (m_Device)
		m_Surface.configure(m_SurfaceConfig);
}

void WebGPUContext::MakeCurrent() {}

wgpu::Device WebGPUContext::GetWebGPUDevice() { return m_Device; }

wgpu::Surface WebGPUContext::GetSurface() { return m_Surface; }

wgpu::TextureFormat WebGPUContext::GetSwapchainFormat() { return m_Surface.getPreferredFormat(m_Adapter); }

wgpu::Queue WebGPUContext::GetQueue() { return m_Queue; }

wgpu::TextureView WebGPUContext::GetCurrentTextureView()
{
	if (!m_SurfaceAcquired)
	{
		m_Surface.getCurrentTexture(&m_CurrentSurfaceTexture);
		if (m_CurrentSurfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success)
			return nullptr;

		wgpu::TextureViewDescriptor viewDescriptor;
		viewDescriptor.format = GetSwapchainFormat();
		viewDescriptor.dimension = wgpu::TextureViewDimension::_2D;
		viewDescriptor.baseMipLevel = 0;
		viewDescriptor.baseArrayLayer = 0;
		viewDescriptor.arrayLayerCount = 1;
		viewDescriptor.mipLevelCount = 1;
		viewDescriptor.aspect = wgpu::TextureAspect::All;
		wgpu::Texture texture = m_CurrentSurfaceTexture.texture;
		m_CurrentTextureView = texture.createView(viewDescriptor);
		m_SurfaceAcquired = true;
	}

	return m_CurrentTextureView;
}

void WebGPUContext::PollEvents()
{
#if defined(WEBGPU_BACKEND_DAWN)
	m_Device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
	m_Device.poll(false);
#endif
}

void WebGPUContext::SetupSurface()
{
	int width, height;
	glfwGetFramebufferSize(m_WindowHandle, &width, &height);
	m_SurfaceConfig.width = width;
	m_SurfaceConfig.height = height;
	m_SurfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
	wgpu::TextureFormat surfaceFormat = m_Surface.getPreferredFormat(m_Adapter);
	m_SurfaceConfig.format = surfaceFormat;

	m_SurfaceConfig.viewFormatCount = 0;
	m_SurfaceConfig.viewFormats = nullptr;
	m_SurfaceConfig.device = m_Device;
	m_SurfaceConfig.alphaMode = wgpu::CompositeAlphaMode::Auto;

	if (width > 0 && height > 0 && m_Device)
		m_Surface.configure(m_SurfaceConfig);
}
