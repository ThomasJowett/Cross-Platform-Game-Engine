#pragma once

#include "Renderer/Texture.h"

class VulkanTexture2D : public Texture2D
{
public:
	VulkanTexture2D(uint32_t width, uint32_t height, Format format, const void* pixels);
	VulkanTexture2D(const std::filesystem::path& filepath);
	virtual ~VulkanTexture2D();

	// Inherited via Texture2D
	virtual uint32_t GetWidth() const override;
	virtual uint32_t GetHeight() const override;
	virtual void SetData(const void* data) override;
	virtual void Bind(uint32_t slot) const override;
	virtual std::string GetName() const override;
	virtual uint32_t GetRendererID() const override;
	virtual void Reload() override;
	virtual bool operator==(const Texture& other) const override;

private:
	void NullTexture();
	uint32_t m_Width, m_Height;
	uint32_t m_RendererID;


};
