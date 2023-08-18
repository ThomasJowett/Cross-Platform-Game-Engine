#include "VulkanSwapChain.h"

#include "Core/Settings.h"
#include <GLFW/glfw3.h>

void VulkanSwapChain::Init(VkInstance instance, Ref<VulkanDevice> device)
{
	m_Instance = instance;
	m_Device = device;
}

void VulkanSwapChain::InitSurface(GLFWwindow* windowHandle)
{
	glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface);
}

void VulkanSwapChain::Create(int* width, int* height)
{
	bool vsync = Settings::GetBool("Display", "V-Sync");
	VkDevice device = m_Device->GetVkDevice();
	VkPhysicalDevice physicalDevice = m_Device->GetPyhsicalDevice()->GetVkPhysicalDevice();

	VkSwapchainKHR oldSwapChain = m_Swapchain;

	VkSurfaceCapabilitiesKHR surfaceCapabilites;

	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilites));

	uint32_t presentModeCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, NULL));

	ASSERT(presentModeCount > 0, "There must be more than one present mode");
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data()));

	//TODO: Fill out the rest of this function
}

void VulkanSwapChain::Destroy()
{
	
}

void VulkanSwapChain::OnResize(int width, int height)
{
	VkDevice device = m_Device->GetVkDevice();
	vkDeviceWaitIdle(device);
	Create(&width, &height);
	vkDeviceWaitIdle(device);
}
