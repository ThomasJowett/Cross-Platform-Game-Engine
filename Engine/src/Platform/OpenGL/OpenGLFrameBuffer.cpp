#include "stdafx.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

void OpenGLFrameBuffer::Bind() const
{
	PROFILE_FUNCTION();

	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);
}

void OpenGLFrameBuffer::UnBind() const
{
	PROFILE_FUNCTION();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Generate(uint32_t width, uint32_t height)
{
	glGenFramebuffers(1, &m_FrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObject);

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		ENGINE_ERROR("FrameBuffer incomplete");
	}

	glBindBuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_FrameBufferObject);
	glDeleteTextures(1, &m_Texture);
}

void OpenGLFrameBuffer::SetTextureSize(uint32_t width, uint32_t height)
{
	Destroy();
	Generate(width, height);
}

uint32_t OpenGLFrameBuffer::GetTextureID()
{
	return m_Texture;
}
