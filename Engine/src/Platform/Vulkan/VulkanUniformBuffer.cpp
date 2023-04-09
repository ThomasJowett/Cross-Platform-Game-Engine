#include "stdafx.h"
#include "VulkanUniformBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
	:m_Size(size), m_Binding(binding)
{

}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
}

void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
}
