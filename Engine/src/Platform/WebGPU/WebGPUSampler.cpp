#include "WebGPUSampler.h"
#include "WebGPUContext.h"
#include "Core/Application.h"

WebGPUSampler::WebGPUSampler(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod)
	:Sampler(filterMethod, wrapMethod)
{
	Ref<WebGPUContext> context = std::dynamic_pointer_cast<WebGPUContext>(Application::GetWindow()->GetContext());
	wgpu::SamplerDescriptor samplerDesc = CreateDescriptor(filterMethod, wrapMethod);
	m_Sampler = context->GetWebGPUDevice().createSampler(samplerDesc);
}

WebGPUSampler::~WebGPUSampler()
{
	if (m_Sampler)
		m_Sampler.release();
}

wgpu::SamplerDescriptor WebGPUSampler::CreateDescriptor(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod)
{
	wgpu::SamplerDescriptor samplerDesc = {};
	switch (filterMethod)
	{
	case Texture::FilterMethod::Linear:
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		samplerDesc.maxAnisotropy = 8;
		break;
	case Texture::FilterMethod::Nearest:
		samplerDesc.minFilter = wgpu::FilterMode::Nearest;
		samplerDesc.magFilter = wgpu::FilterMode::Nearest;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
		samplerDesc.maxAnisotropy = 1;
		break;
	default:
		break;
	}

	switch (wrapMethod)
	{
	case Texture::WrapMethod::Clamp:
		samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
		samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
		samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
		break;
	case Texture::WrapMethod::Mirror:
		samplerDesc.addressModeU = wgpu::AddressMode::MirrorRepeat;
		samplerDesc.addressModeV = wgpu::AddressMode::MirrorRepeat;
		samplerDesc.addressModeW = wgpu::AddressMode::MirrorRepeat;
		break;
	case Texture::WrapMethod::Repeat:
		samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeW = wgpu::AddressMode::Repeat;
		break;
	default:
		break;
	}

	return samplerDesc;
}
