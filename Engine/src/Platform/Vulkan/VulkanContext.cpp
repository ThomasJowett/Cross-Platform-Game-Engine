#include "stdafx.h"
#include "VulkanContext.h"
#include "Core/Version.h"
#include "Logging/Instrumentor.h"

#include "Vulkan.h"

#include "GLFW/glfw3.h"

VkInstance g_VkInstance = VK_NULL_HANDLE;

VulkanContext::VulkanContext(GLFWwindow* windowHandle)
	:m_WindowHandle(windowHandle)
{
}

VulkanContext::~VulkanContext()
{
	vkDestroyInstance(g_VkInstance, nullptr);
}

void VulkanContext::Init()
{
	PROFILE_FUNCTION();

	if (volkInitialize() != VK_SUCCESS)
	{
		ENGINE_CRITICAL("Volk failed to initialize");
	}

	if (volkGetInstanceVersion() == 0)
	{
		ENGINE_CRITICAL("Could not find volk loader");
	}

	VkResult err;
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Cross-Platform-Game-Engine";
	appInfo.pEngineName = "Cross-Platform-Game-Engine";
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.applicationVersion = VERSION;

	std::vector<const char*>instanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME
	};

#if defined(_WIN32)
	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
	instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(_DIRECT2DISPLAY)
	instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#if defined(VK_USE_PLATFORM_MACOS_MVK) && (VK_HEADER_VERSION >= 216)
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

	VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT };
	VkValidationFeaturesEXT features = {};
	features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	features.enabledValidationFeatureCount = 1;
	features.pEnabledValidationFeatures = enables;

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
#if defined(VK_USE_PLATFORM_MACOS_MVK) && (VK_HEADER_VERSION >= 216)
    instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

	err = vkCreateInstance(&instanceCreateInfo, nullptr, &g_VkInstance);

	if (err != VK_SUCCESS) {
		ENGINE_CRITICAL("Could not create vulkan instance!");
		return;
	}

	volkLoadInstance(g_VkInstance);

	m_PhysicalDevice = CreateRef<VulkanPhysicalDevice>();

	VkPhysicalDeviceFeatures enabledFeatures{};
	if (m_PhysicalDevice->GetVkPhysicalDeviceFeatures().samplerAnisotropy) {
		enabledFeatures.samplerAnisotropy = VK_TRUE;
	}

	m_Device = CreateRef<VulkanDevice>(m_PhysicalDevice, enabledFeatures);
	m_Swapchain = CreateRef<VulkanSwapChain>();
	m_Swapchain->Init(g_VkInstance, m_Device);
	m_Swapchain->InitSurface(m_WindowHandle);

	int width, height;
	glfwGetWindowSize(m_WindowHandle, &width, &height);
	m_Swapchain->Create(&width, &height);
}

void VulkanContext::SwapBuffers()
{
	PROFILE_FUNCTION();
	glfwSwapBuffers(m_WindowHandle);
}

void VulkanContext::ResizeBuffers(uint32_t width, uint32_t height)
{
	m_Swapchain->OnResize(width, height);
}

void VulkanContext::SetSwapInterval(uint32_t interval)
{
	glfwSwapInterval((int)interval);
}

void VulkanContext::MakeCurrent()
{
	glfwMakeContextCurrent(m_WindowHandle);
}