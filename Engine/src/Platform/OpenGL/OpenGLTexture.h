#pragma once

#include "Renderer/Texture.h"
#include <glad/glad.h>

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(uint32_t width, uint32_t height);
	OpenGLTexture2D(const std::filesystem::path& filepath);
	virtual ~OpenGLTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(void* data, uint32_t size) override;

	virtual void Bind(uint32_t slot) const override;

	virtual std::string GetName() const override;

	virtual const std::filesystem::path& GetFilepath() const override;

	virtual uint32_t GetRendererID() const override;

	virtual bool operator==(const Texture& other) const override;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();
	std::filesystem::path m_Path;
	uint32_t m_Width, m_Height;

	uint32_t m_RendererID;

	GLenum m_InternalFormat, m_DataFormat;
};
