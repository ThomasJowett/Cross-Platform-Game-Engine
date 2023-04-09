#include "stdafx.h"
#include "VulkanDevice.h"

bool PhysicalDevice::IsExtensionSupported(const std::string& extensionName) const
{
    return m_SupportedExtensions.find(extensionName) != m_SupportedExtensions.end();
}
