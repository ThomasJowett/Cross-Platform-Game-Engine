#include "stdafx.h"
#include "OpenGLBuffer.h"

#include "Core/Application.h"

#include <glad/glad.h>

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	CORE_ASSERT(size < m_Size, "Size must be less than the buffer size");

	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void OpenGLVertexBuffer::SetData(const void* data)
{
	SetData(data, m_Size);
}

void OpenGLVertexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGLVertexBuffer::UnBind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//------------------------------------------------------------------------------------

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
	:m_Count(count)
{
	PROFILE_FUNCTION();
	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void OpenGLIndexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGLIndexBuffer::UnBind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
