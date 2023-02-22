#include "stdafx.h"
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

void OpenGLPipeline::Bind()
{
}