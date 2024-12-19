#pragma once

#include "Renderer/Texture.h"
#include <glad/glad.h>

class WebGPUTexture2D : public Texture2D
{
public:
	WebGPUTexture2D(uint32_t width, uint32_t height, Format format, const void* pixels);
	WebGPUTexture2D(const std::filesystem::path& filepath);
	virtual ~WebGPUTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(const void* data) override;

	virtual void Bind(uint32_t slot) const override;

	virtual std::string GetName() const override;

	virtual uint32_t GetRendererID() const override;

	virtual void Reload() override;

	virtual bool operator==(const Texture& other) const override;

	virtual void SetFilterMethod(FilterMethod filterMethod) override;
	virtual void SetWrapMethod(WrapMethod wrapMethod) override;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();

	void SetFilteringAndWrappingMethod();
	uint32_t m_Width, m_Height;

	uint32_t m_RendererID;

	GLenum m_InternalFormat, m_DataFormat, m_Type;
};
