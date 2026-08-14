#include "OpenGLPipeline.h"

#include <glad/glad.h>

OpenGLPipeline::OpenGLPipeline(const Spec& spec)
{
	m_TransparencyEnabled = spec.transparencyEnabled;
	m_BackfaceCull = spec.backFaceCulling;
	m_Specification = spec;
}

OpenGLPipeline::~OpenGLPipeline()
{
}

void OpenGLPipeline::Invalidate()
{
}

void OpenGLPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
{
}
void OpenGLPipeline::SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set)
{
	if (texture)
		texture->Bind(binding);
}

void OpenGLPipeline::Bind()
{
	if (m_Specification.hasDepth)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(m_Specification.depthTest ? GL_LESS : GL_ALWAYS);
		glDepthMask(m_Specification.depthTest ? GL_TRUE : GL_FALSE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}