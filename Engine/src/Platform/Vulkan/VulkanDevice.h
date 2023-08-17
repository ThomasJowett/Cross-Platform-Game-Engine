#pragma once

#include "Vulkan.h"
#include <string>

#include "Core/core.h"

class VulkanPhysicalDevice
{
	struct QueueFamilyIndices {
		uint32_t Graphics;
		uint32_t Compute;
		uint32_t Transfer;
	};
public:
	VulkanPhysicalDevice();
	bool IsExtensionSupported(const std::string& extensionName) const;

	VkPhysicalDevice GetVkPhysicalDevice() { return m_PhysicalDevice; }
	const std::vector<VkDeviceQueueCreateInfo>& GetDeviceQueueCreateInfos() { return m_QueueCreateInfos; }
	VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures() { return m_Features; }

	uint32_t GetGraphicsQueueFamilyIndex() { return m_QueueFamilyIndices.Graphics; }
	uint32_t GetComputeQueueFamilyIndex() { return m_QueueFamilyIndices.Compute; }
	uint32_t GetTransferQueueFamilyIndex() { return m_QueueFamilyIndices.Transfer; }
private:
	uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);
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
