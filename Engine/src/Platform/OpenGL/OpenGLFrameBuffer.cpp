#include "stdafx.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

static GLenum TextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
{
	glCreateTextures(TextureTarget(multisampled), count, outID);
}

static void BindTexture(bool multisampled, uint32_t id)
{
	glBindTexture(TextureTarget(multisampled), id);
}

static void AttachColourTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
}

static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification), m_RendererID(0), m_DepthAttachment(0)
{
	for (auto spec : m_Specification.Attachments.Attachments)
	{
		if (!IsDepthFormat(spec.TextureFormat))
		{
			m_ColourAttachmentSpecifications.emplace_back(spec);

		}
		else
			m_DepthAttachmentSpecification = spec;
	}
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

	bool multisample = m_Specification.Samples > 1;

	// Attachments

	if (m_ColourAttachmentSpecifications.size())
	{
		m_ColourAttachments.resize(m_ColourAttachmentSpecifications.size());

		CreateTextures(multisample, m_ColourAttachments.data(), m_ColourAttachments.size());
		for (size_t i = 0; i < m_ColourAttachmentSpecifications.size(); i++)
		{
			BindTexture(multisample, m_ColourAttachments[i]);
			switch (m_ColourAttachmentSpecifications[i].TextureFormat)
			{
			case FrameBufferTextureFormat::RGBA8:
				AttachColourTexture(m_ColourAttachments[i], m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, i);
				break;
			default:
				break;
			}
		}
	}

	if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
	{
		CreateTextures(multisample, &m_DepthAttachment, 1);
		BindTexture(multisample, m_DepthAttachment);
		switch (m_DepthAttachmentSpecification.TextureFormat)
		{
		case FrameBufferTextureFormat::DEPTH24STENCIL8:
			AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
			break;
		default:
			break;
		}
	}

	if (m_ColourAttachments.size() > 1)
	{
		CORE_ASSERT(m_ColourAttachments.size() <= 4, "Framebuffer cannot have more than 4 attachments");
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers((GLsizei)m_ColourAttachments.size(), buffers);
	}
	else if (m_ColourAttachments.empty())
	{
		glDrawBuffer(GL_NONE);
	}

	CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures((GLsizei)m_ColourAttachments.size(), m_ColourAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);
	m_ColourAttachments.clear();
	m_DepthAttachment = 0;
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	m_Specification.Width = width;
	m_Specification.Height = height;
	Destroy();
	Generate();
}

uint32_t OpenGLFrameBuffer::GetColourAttachment(size_t index)
{
	CORE_ASSERT(index < m_ColourAttachments.size(), "Index out of range");
	return m_ColourAttachments[index];
}
