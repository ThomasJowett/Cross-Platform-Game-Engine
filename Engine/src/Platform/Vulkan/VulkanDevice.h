#pragma once

#include <vulkan/vulkan.h>
#include <string>

class PhysicalDevice
{
	bool IsExtensionSupported(const std::string& extensionName) const;
private:
	VkPhysicalDevice m_PhysicalDevice = nullptr;
	VkPhysicalDeviceProperties m_Properties;
	VkPhysicalDeviceFeatures m_Features;
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;

	std::unordered_set<std::string> m_SupportedExtensions;
};

class VulkanDevice
{
private:
	VkDevice m_Device;
};
