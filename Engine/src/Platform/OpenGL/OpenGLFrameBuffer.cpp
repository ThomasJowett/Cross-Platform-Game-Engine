#include "stdafx.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification)
{
	Generate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	Destroy();
}

void OpenGLFrameBuffer::Bind()
{
	PROFILE_FUNCTION();

	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLFrameBuffer::UnBind()
{
	PROFILE_FUNCTION();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Generate()
{
	PROFILE_FUNCTION();

	if (m_RendererID)
		Destroy();

	if (m_Specification.Height == 0 || m_Specification.Width == 0)
	{
		Destroy();
		return;
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColourAttachment);
	glBindTexture(GL_TEXTURE_2D, m_ColourAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

	CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(1, &m_ColourAttachment);
	glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	m_Specification.Width = width;
	m_Specification.Height = height;
	Destroy();
	Generate();
}

uint32_t OpenGLFrameBuffer::GetColourAttachment()
{
	return m_ColourAttachment;
}
