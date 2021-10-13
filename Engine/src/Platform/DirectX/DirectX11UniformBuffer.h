#pragma once

#include "Renderer/UniformBuffer.h"

class DirectX11UniformBuffer : public UniformBuffer
{
public:
	DirectX11UniformBuffer(uint32_t size, uint32_t binding);
	virtual ~DirectX11UniformBuffer();

	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
private:
	uint32_t m_RendererID = 0;
};