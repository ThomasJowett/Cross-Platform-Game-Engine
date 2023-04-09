#pragma once

#include "Renderer/UniformBuffer.h"

class VulkanUniformBuffer : public UniformBuffer
{
public:
	VulkanUniformBuffer(uint32_t size, uint32_t binding);
	virtual ~VulkanUniformBuffer();


	// Inherited via UniformBuffer
	virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;
private:
	uint32_t m_Size = 0;
	uint32_t m_Binding = 0;
};
