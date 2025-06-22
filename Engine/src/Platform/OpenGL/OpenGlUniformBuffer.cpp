#include "stdafx.h"
#include "OpenGlUniformBuffer.h"
#include "Core/Application.h"

#include <glad/glad.h>

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &m_RendererID);
	glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(m_RendererID, offset, size, data);
}