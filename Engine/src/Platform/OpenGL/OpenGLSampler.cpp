#include "OpenGLSampler.h"

#include <glad/glad.h>

OpenGLSampler::OpenGLSampler(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod)
	:Sampler(filterMethod, wrapMethod)
{
	glCreateSamplers(1, &m_RendererID);

	GLint filter = filterMethod == Texture::FilterMethod::Linear ? GL_LINEAR : GL_NEAREST;
	glSamplerParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, filter);
	glSamplerParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, filter);
	glSamplerParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, filterMethod == Texture::FilterMethod::Linear ? 8.0f : 1.0f);

	GLint wrap = GL_REPEAT;
	if (wrapMethod == Texture::WrapMethod::Clamp)
		wrap = GL_CLAMP_TO_EDGE;
	else if (wrapMethod == Texture::WrapMethod::Mirror)
		wrap = GL_MIRRORED_REPEAT;
	glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_S, wrap);
	glSamplerParameteri(m_RendererID, GL_TEXTURE_WRAP_T, wrap);
}

OpenGLSampler::~OpenGLSampler()
{
	glDeleteSamplers(1, &m_RendererID);
}

void OpenGLSampler::Bind(uint32_t unit) const
{
	glBindSampler(unit, m_RendererID);
}
