#include "stdafx.h"
#include "VulkanDevice.h"

extern VkInstance g_VkInstance;

VulkanPhysicalDevice::VulkanPhysicalDevice()
{
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(g_VkInstance, &gpuCount, nullptr);

	ASSERT(gpuCount > 0, "No vulkan physical rendering devices found");

	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
	vkEnumeratePhysicalDevices(g_VkInstance, &gpuCount, physicalDevices.data());

	VkPhysicalDevice selectedPhysicalDevice = nullptr;
	for (VkPhysicalDevice physicalDevice : physicalDevices)
	{
		vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);

		if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			selectedPhysicalDevice = physicalDevice;
			break;
		}
	}

	if (!selectedPhysicalDevice)
	{
		for (VkPhysicalDevice physicalDevice : physicalDevices)
		{
			vkGetPhysicalDeviceProperties(physicalDevice, &m_Properties);

			if (m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			{
				selectedPhysicalDevice = physicalDevice;
				break;
			}
		}
	}

	ASSERT(selectedPhysicalDevice, "Could not find suitable vulkan physical rendering device");

	m_PhysicalDevice = selectedPhysicalDevice;

	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

	ENGINE_INFO("Vulkan : {0}.{1}.{2}", VK_API_VERSION_MAJOR(m_Properties.apiVersion), VK_API_VERSION_MINOR(m_Properties.apiVersion), VK_API_VERSION_PATCH(m_Properties.apiVersion));
	ENGINE_INFO("GPU : {0} {1}", m_Properties.deviceName, m_Properties.deviceType);

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

	ASSERT(queueFamilyCount > 0, "Family count must be greater than zero");

	m_QueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

	std::vector<VkBool32>supportsPresent(queueFamilyCount);
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		
		//fpGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, surface, &supportsPresent[i]);
	}

	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if (graphicsQueueNodeIndex == UINT32_MAX)
			{
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE)
			{
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX)
	{
		// If there's no queue that supports both present and graphics
		// try to find a separate present queue
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			if (supportsPresent[i] == VK_TRUE)
			{
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	uint32_t extCount = 0;
}

bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
{
	return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
}

VulkanDevice::VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
	:m_PhysicalDevice(physicalDevice), m_EnabledFeatures(enabledFeatures)
{
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//deviceCreateInfo.queueCreateInfoCount = (uint32_t)m_PhysicalDevice->
	VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device);

	//vkGetDeviceQueue(m_Device, )
}

VulkanDevice::~VulkanDevice()
{
	if (m_Device) {
		vkDestroyDevice(m_Device, nullptr);
	}
}
