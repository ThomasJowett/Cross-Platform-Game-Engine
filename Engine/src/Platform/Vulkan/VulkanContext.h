#pragma once

#include "Renderer/GraphicsContext.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "Core/core.h"

class VulkanContext : public GraphicsContext
{
public:
	VulkanContext(GLFWwindow* windowHandle);
	~VulkanContext();

	// Inherited via GraphicsContext
	virtual void Init() override;

	virtual void SwapBuffers() override;

	virtual void ResizeBuffers(uint32_t width, uint32_t height) override;

	virtual void SetSwapInterval(uint32_t interval) override;

	virtual void MakeCurrent() override;

	Ref<VulkanPhysicalDevice> GetPhysicalDevice() { return m_PhysicalDevice; }
	Ref<VulkanDevice> GetDevice() { return m_Device; }

private:
	GLFWwindow* m_WindowHandle;
	Ref<VulkanPhysicalDevice> m_PhysicalDevice;
	Ref<VulkanDevice> m_Device;
	Ref<VulkanSwapChain> m_Swapchain;
};
