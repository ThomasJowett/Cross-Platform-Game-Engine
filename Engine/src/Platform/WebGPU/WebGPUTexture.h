#pragma once

#include "Asset/Texture.h"
#include "WebGPUContext.h"
#include <webgpu/webgpu.hpp>

class WebGPUTexture2D : public Texture2D
{
public:
	WebGPUTexture2D(uint32_t width, uint32_t height, Format format, uint32_t samples, const void* pixels);
	WebGPUTexture2D(const std::filesystem::path& filepath);
	WebGPUTexture2D(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData);
	virtual ~WebGPUTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(const void* data) override;

	virtual void Bind(uint32_t slot) const override;

	virtual std::string GetName() const override;

	virtual void* GetRendererID() const override;

	virtual bool Reload() override;

	virtual bool operator==(const Texture& other) const override;

	virtual void SetFilterMethod(FilterMethod filterMethod) override;
	virtual void SetWrapMethod(WrapMethod wrapMethod) override;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();

	void CreateSampler();
	uint32_t m_Width, m_Height;

	wgpu::TextureFormat m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;
	wgpu::TextureDescriptor m_TextureDesc;
	wgpu::Texture m_Texture;

	wgpu::TextureViewDescriptor m_TextureViewDesc;
	wgpu::TextureView m_TextureView;

	wgpu::Sampler m_Sampler;

	wgpu::BindGroup m_BindGroup;

	Ref<WebGPUContext> m_WebGPUContext;
};
