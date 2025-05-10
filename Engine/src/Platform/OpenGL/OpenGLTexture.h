#pragma once

#include "Asset/Texture.h"
#include <glad/glad.h>

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(uint32_t width, uint32_t height, Format format, const void* pixels);
	OpenGLTexture2D(const std::filesystem::path& filepath);
	OpenGLTexture2D(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData);
	virtual ~OpenGLTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(const void* data) override;

	virtual void Bind(uint32_t slot) const override;

	virtual uint32_t GetRendererID() const override;

	virtual void Reload() override;

	virtual bool operator==(const Texture& other) const override;

	virtual void SetFilterMethod(FilterMethod filterMethod) override;
	virtual void SetWrapMethod(WrapMethod wrapMethod) override;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();

	bool LoadTextureFromMemory(const std::vector<uint8_t>& imageData);

	void SetFilteringAndWrappingMethod();
	uint32_t m_Width, m_Height;

	uint32_t m_RendererID;

	GLenum m_InternalFormat, m_DataFormat, m_Type;
};
