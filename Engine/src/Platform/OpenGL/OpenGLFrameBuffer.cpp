#include "stdafx.h"
#include "OpenGLFrameBuffer.h"
#include "Logging/Instrumentor.h"

#include <glad/glad.h>

static GLenum TextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, uint32_t* outID, size_t count)
{
	glCreateTextures(TextureTarget(multisampled), (GLsizei)count, outID);
}

static void BindTexture(bool multisampled, uint32_t id)
{
	glBindTexture(TextureTarget(multisampled), id);
}

static void AttachColourTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, size_t index)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)index, TextureTarget(multisampled), id, 0);
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
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}

static GLenum TextureFormatToOpenGlTextureFormat(FrameBufferTextureFormat format)
{
	switch (format)
	{
	case FrameBufferTextureFormat::RGBA8:
		return GL_RGBA8;
	case FrameBufferTextureFormat::RED_INTEGER:
		return GL_RED_INTEGER;
	case FrameBufferTextureFormat::DEPTH24STENCIL8:
		return GL_DEPTH24_STENCIL8;
	default:
		return GLenum();
	}
}

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification), m_RendererID(0), m_DepthAttachment(0)
{
	for (auto spec : m_Specification.attachments.attachments)
	{
		if (!IsDepthFormat(spec.textureFormat))
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
	glViewport(0, 0, m_Specification.width, m_Specification.height);
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

	if (m_Specification.height == 0 || m_Specification.width == 0)
	{
		Destroy();
		return;
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	bool multisample = m_Specification.samples > 1;

	// Attachments

	if (!m_ColourAttachmentSpecifications.empty())
	{
		m_ColourAttachments.resize(m_ColourAttachmentSpecifications.size());

		for (size_t i = 0; i < m_ColourAttachmentSpecifications.size(); i++)
		{
			auto format = FrameBufferFormatToTextureFormat(m_ColourAttachmentSpecifications[i].textureFormat);

			m_ColourAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, format, m_Specification.samples);

			BindTexture(multisample, m_ColourAttachments[i]->GetRendererID());
			switch (m_ColourAttachmentSpecifications[i].textureFormat)
			{
			case FrameBufferTextureFormat::RGBA8:
				AttachColourTexture(m_ColourAttachments[i]->GetRendererID(), m_Specification.samples, GL_RGBA8, GL_RGBA, m_Specification.width, m_Specification.height, i);
				break;
			case FrameBufferTextureFormat::RED_INTEGER:
				AttachColourTexture(m_ColourAttachments[i]->GetRendererID(), m_Specification.samples, GL_R32I, GL_RED_INTEGER, m_Specification.width, m_Specification.height, i);
				break;
			default:
				break;
			}

			m_ColourAttachments[i]->SetFilterMethod(Texture::FilterMethod::Linear);
			m_ColourAttachments[i]->SetWrapMethod(Texture::WrapMethod::Clamp);
		}
	}
	
	if (m_DepthAttachmentSpecification.textureFormat != FrameBufferTextureFormat::None)
	{
		auto format = FrameBufferFormatToTextureFormat(m_DepthAttachmentSpecification.textureFormat);
		m_DepthAttachment = Texture2D::Create(m_Specification.width, m_Specification.height, format, m_Specification.samples);
	
		BindTexture(multisample, m_DepthAttachment->GetRendererID());
		switch (m_DepthAttachmentSpecification.textureFormat)
		{
		case FrameBufferTextureFormat::DEPTH24STENCIL8:
			AttachDepthTexture(m_DepthAttachment->GetRendererID(), m_Specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.width, m_Specification.height);
			break;
		default:
			break;
		}

		m_DepthAttachment->SetFilterMethod(Texture::FilterMethod::Linear);
		m_DepthAttachment->SetWrapMethod(Texture::WrapMethod::Clamp);
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

	m_ColourAttachments.clear();
	m_DepthAttachment.reset();
}

void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	m_Specification.width = width;
	m_Specification.height = height;
	Destroy();
	Generate();
}

int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	CORE_ASSERT(attachmentIndex < m_ColourAttachments.size(), "Trying to access attachment that does not exist!");
	glReadBuffer(GL_COLOR_ATTACHMENT0 + (int)attachmentIndex);
	int pixelData;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;
}

Ref<Texture> OpenGLFrameBuffer::GetColourAttachment(size_t index)
{
	CORE_ASSERT(index < m_ColourAttachments.size(), "Index out of range");
	return m_ColourAttachments[index];
}

void OpenGLFrameBuffer::ClearAttachment(size_t index, int value)
{
	CORE_ASSERT(index < m_ColourAttachments.size(), "Trying to access attachment that does not exist!");

	FrameBufferTextureSpecification& spec = m_ColourAttachmentSpecifications[index];

	GLenum textureFormat = TextureFormatToOpenGlTextureFormat(spec.textureFormat);

	glClearTexImage(m_ColourAttachments[index]->GetRendererID(), 0, textureFormat, GL_INT, &value);
}

