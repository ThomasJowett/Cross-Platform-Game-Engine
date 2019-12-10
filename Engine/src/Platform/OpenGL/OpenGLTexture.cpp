#include "stdafx.h"
#include "OpenGLTexture.h"
#include "GLAD/glad.h"

#include "stb/stb_image.h"

OpenGLTexture2D::OpenGLTexture2D(std::string path)
	:m_Path(path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	CORE_ASSERT(data, "Failed to load image!");

	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
	glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

	glTexParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
	glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
	glBindTextureUnit(slot, m_RendererID);
}
