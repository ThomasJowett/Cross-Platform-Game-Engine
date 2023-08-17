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

	int requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	m_QueueFamilyIndices = GetQueueFamilyIndices(requestedQueueTypes);

	// Graphics queue
	if(requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
		VkDeviceQueueCreateInfo queueInfo{};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &defaultQueuePriority;
		m_QueueCreateInfos.push_back(queueInfo);
	}

	// Dedicated compute queue
	if(requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
		if(m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics){
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(queueInfo);
		}
	}

	// Dedicated transfer queue
	if(requestedQueueTypes & VK_QUEUE_TRANSFER_BIT) {
		if((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute) && m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics){
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			m_QueueCreateInfos.push_back(queueInfo);
		}
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
	deviceCreateInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

	if(deviceExtensions.size() > 0) {
		deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device);

	if (result == VK_SUCCESS) {
		//TODO create the command pool
	}
}

VulkanDevice::~VulkanDevice()
{
	if (m_Device) {
		vkDestroyDevice(m_Device, nullptr);
	}
}

VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(int queueFlags)
{
	QueueFamilyIndices indices;

	// Dedicated compute queue
	if(queueFlags & VK_QUEUE_COMPUTE_BIT) {
		for(uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++){
			VkQueueFamilyProperties& queueFamilyProperties = m_QueueFamilyProperties[i];
			if((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)){
				indices.Compute = i;
				break;
			}
		}
	}

	// Dedicated transfer queue
	if(queueFlags & VK_QUEUE_TRANSFER_BIT) {
		for(uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++) {
			VkQueueFamilyProperties& queueFamilyProperties = m_QueueFamilyProperties[i];
			if((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				indices.Transfer = i;
				break;
			}
		}
	}

	for(uint32_t i = 0; i < m_QueueFamilyProperties.size(); i++){
		if((queueFlags & VK_QUEUE_TRANSFER_BIT) && indices.Transfer == -1)
		{
			if(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
				indices.Transfer = i;
		}

		if((queueFlags & VK_QUEUE_COMPUTE_BIT) && indices.Compute == -1){
			if(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
				indices.Compute = i;
		}

		if(queueFlags & VK_QUEUE_GRAPHICS_BIT){
			if(m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.Compute = i;
		}
	}

	return indices;
}
