#include "stdafx.h"
#include "VulkanBuffer.h"

#include "Vulkan.h"

#include "Logging/Instrumentor.h"

uint32_t FindMemoryType(VkDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;

	//vkGetPhysicalDeviceMemoryProperties(VulkanContext::);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	ENGINE_ERROR("Failed to find suitable Vulkan memory type");
	return 0;
}

VkBuffer CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBuffer buffer;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		ENGINE_ERROR("Could not create a Vulkan Buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, properties);

	VkDeviceMemory bufferMemory;
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		ENGINE_ERROR("Failed to allocate Vulkan buffer memory");

	vkBindBufferMemory(device, buffer, bufferMemory, 0);

	return buffer;
}

VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
{
}

VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32_t size)
{
	VkDeviceSize bufferSize = sizeof(float) * size;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
}

void VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
{
}

const BufferLayout& VulkanVertexBuffer::GetLayout() const
{
	return m_Layout;
}

void VulkanVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
}

void VulkanVertexBuffer::SetData(const void* data)
{
}

uint32_t VulkanVertexBuffer::GetSize()
{
	return 0;
}

void VulkanVertexBuffer::Bind() const
{
}

void VulkanVertexBuffer::UnBind() const
{
}

//------------------------------------------------------------------------------------

VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
	:m_Count(count)
{
	PROFILE_FUNCTION();

}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
}

void VulkanIndexBuffer::Bind() const
{
}

void VulkanIndexBuffer::UnBind() const
{
}

uint32_t VulkanIndexBuffer::GetCount() const
{
	return 0;
}
