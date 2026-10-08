#pragma once

#include "Renderer/Sampler.h"

#include <webgpu/webgpu.hpp>

class WebGPUSampler : public Sampler
{
public:
	WebGPUSampler(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod);
	virtual ~WebGPUSampler();

	const wgpu::Sampler& GetSampler() const { return m_Sampler; }

	static wgpu::SamplerDescriptor CreateDescriptor(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod);

private:
	wgpu::Sampler m_Sampler;
};
