#pragma once

#include "Vulkan.h"
#include <string>

#include "Core/core.h"

class VulkanPhysicalDevice
{
	struct QueueFamilyIndices {
		uint32_t graphics;
		uint32_t compute;
	};
public:
	VulkanPhysicalDevice();
	bool IsExtensionSupported(const std::string& extensionName) const;

	VkPhysicalDevice GetVkPhysicalDevice() { return m_PhysicalDevice; }
private:
	QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
private:
	VkPhysicalDevice m_PhysicalDevice = nullptr;
	VkPhysicalDeviceProperties m_Properties;
	VkPhysicalDeviceFeatures m_Features;
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;

	std::unordered_set<std::string> m_SupportedExtensions;

	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
	QueueFamilyIndices m_QueueFamilyIndices;
};

class VulkanDevice
{
public:
	VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures);
	~VulkanDevice();
	VkDevice GetVkDevice() { return m_Device; }

	VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
private:
private:
	VkDevice m_Device;
	Ref<VulkanPhysicalDevice> m_PhysicalDevice;
	VkPhysicalDeviceFeatures m_EnabledFeatures;

	VkQueue m_GraphicsQueue;

};
