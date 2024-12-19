#include "stdafx.h"
#include "VulkanDevice.h"

#include <GLFW/glfw3.h>

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
	
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

	if (extensionCount > 0) {
		std::vector<VkExtensionProperties> extensions(extensionCount);
		if (vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, &extensions.front()) == VK_SUCCESS) {
			ENGINE_INFO("Selected physical device has {0} extensions", extensions.size());
			for (const VkExtensionProperties& extension : extensions) {
				m_SupportedExtensions.emplace(extension.extensionName);
			}
		}
	}

	static const float defaultQueuePriority(0.0f);

	int requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

	// Graphics queue
	if(requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
		m_QueueFamilyIndices.Graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		m_QueueCreateInfos.push_back(queueInfo);
	}
	else {
		m_QueueFamilyIndices.Graphics = 0;
	}

	// Dedicated compute queue
	if(requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
		m_QueueFamilyIndices.Compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
		if(m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics){
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(queueInfo);
		}
	}
	else {
		m_QueueFamilyIndices.Compute = m_QueueFamilyIndices.Graphics;
	}

	std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);
		if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			m_DepthFormat = format;
	}
}

bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
{
	return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
}

VulkanDevice::VulkanDevice(const Ref<VulkanPhysicalDevice> physicalDevice, VkPhysicalDeviceFeatures enabledFeatures)
	:m_PhysicalDevice(physicalDevice), m_EnabledFeatures(enabledFeatures)
{
	std::vector<const char*> deviceExtensions{};
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos = m_PhysicalDevice->GetDeviceQueueCreateInfos();
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
	
	if(deviceExtensions.size() > 0) {
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device);

	vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetGraphicsQueueFamilyIndex(), 0, &m_GraphicsQueue);

	if (result == VK_SUCCESS) {
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = m_PhysicalDevice->GetGraphicsQueueFamilyIndex();
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &cmdPoolInfo, nullptr, &m_CommandPool));

		cmdPoolInfo.queueFamilyIndex = m_PhysicalDevice->GetComputeQueueFamilyIndex();
		VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &cmdPoolInfo, nullptr, &m_ComputeCommandPool));

		vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetGraphicsQueueFamilyIndex(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetComputeQueueFamilyIndex(), 0, &m_ComputeQueue);
	}
}

VulkanDevice::~VulkanDevice()
{
	if (m_Device) {
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		vkDestroyCommandPool(m_Device, m_ComputeCommandPool, nullptr);

		vkDeviceWaitIdle(m_Device);
		vkDestroyDevice(m_Device, nullptr);
	}
}

VkCommandBuffer VulkanDevice::GetCommandBuffer(bool begin, bool compute)
{
	VkCommandBuffer cmdBuffer;

	VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
	cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocateInfo.commandPool = compute ? m_ComputeCommandPool : m_CommandPool;
	cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufAllocateInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &cmdBufAllocateInfo, &cmdBuffer));

	if (begin)
	{
		VkCommandBufferBeginInfo cmdBufBeginInfo{};
		cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBufBeginInfo));
	}
	return cmdBuffer;
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer)
{
	FlushCommandBuffer(commandBuffer, m_GraphicsQueue);
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
{
	ASSERT(commandBuffer != VK_NULL_HANDLE, "Command buffer is null");
	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	VkFence fence;
	VK_CHECK_RESULT(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &fence));

	// Submit to the queue
	VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, fence));
	// Wait for the fence to signal that command buffer has finished executing
	VK_CHECK_RESULT(vkWaitForFences(m_Device, 1, &fence, VK_TRUE, UINT64_MAX));

	vkDestroyFence(m_Device, fence, nullptr);
	vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
}

VkCommandBuffer VulkanDevice::CreateSecondaryCommandBuffer() const
{
	VkCommandBuffer cmdBuffer;

	VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
	cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufAllocateInfo.commandPool = m_CommandPool;
	cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	cmdBufAllocateInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(m_Device, &cmdBufAllocateInfo, &cmdBuffer));
	return cmdBuffer;
}

uint32_t VulkanPhysicalDevice::GetQueueFamilyIndex(VkQueueFlagBits queueFlags)
{
	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++) {
			if ((m_QueueFamilyProperties[i].queueFlags & queueFlags) && ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
				return i;
				break;
			}
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(m_QueueFamilyProperties.size()); i++) {
		if (m_QueueFamilyProperties[i].queueFlags & queueFlags) {
			return i;
			break;
		}
	}

	ENGINE_ERROR("Could not find a matching queue family index");
	return 0;
}
