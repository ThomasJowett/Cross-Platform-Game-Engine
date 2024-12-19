#include "stdafx.h"
#include "WebGPUUniformBuffer.h"
#include "Core/Application.h"

#include <glad/glad.h>

WebGPUUniformBuffer::WebGPUUniformBuffer(uint32_t size, uint32_t binding)
{
	glCreateBuffers(1, &m_RendererID);
	glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

WebGPUUniformBuffer::~WebGPUUniformBuffer()
{
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void WebGPUUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	glNamedBufferSubData(m_RendererID, offset, size, data);
}
