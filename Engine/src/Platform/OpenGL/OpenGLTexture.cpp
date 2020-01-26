#include "stdafx.h"
#include "OpenGLTexture.h"

#include <stb/stb_image.h>

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
	:m_Width(width), m_Height(height)
{
	PROFILE_FUNCTION();

	m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
	:m_Path(path)
{
	PROFILE_FUNCTION();

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		PROFILE_SCOPE("stbi Load Image OpenGLTexture2D(const std::string&)");
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	}

	CORE_ASSERT(data, "Failed to load image!");

	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	CORE_ASSERT(internalFormat && dataFormat, "Format not supported");

	m_InternalFormat = internalFormat;
	m_DataFormat = dataFormat;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	PROFILE_FUNCTION();
	glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::SetData(void * data, uint32_t size)
{
	PROFILE_FUNCTION();
	uint32_t bytePerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
	CORE_ASSERT(size == m_Width * m_Height * bytePerPixel, "Data must be entire texture")
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
	PROFILE_FUNCTION();
	glBindTextureUnit(slot, m_RendererID);
}
