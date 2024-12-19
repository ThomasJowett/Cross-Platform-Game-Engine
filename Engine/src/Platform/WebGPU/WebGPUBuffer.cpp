#include "stdafx.h"
#include "WebGPUBuffer.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include <glad/glad.h>

WebGPUVertexBuffer::WebGPUVertexBuffer(uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	m_VertexArray = CreateRef<WebGPUVertexArray>();
}

WebGPUVertexBuffer::WebGPUVertexBuffer(void* vertices, uint32_t size)
	:m_Size(size)
{
	PROFILE_FUNCTION();
    glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	m_VertexArray = CreateRef<WebGPUVertexArray>();
}

WebGPUVertexBuffer::~WebGPUVertexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void WebGPUVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	CORE_ASSERT(size <= m_Size, "Size must be less than the buffer size");

	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void WebGPUVertexBuffer::SetData(const void* data)
{
	SetData(data, m_Size);
}

void WebGPUVertexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	m_VertexArray->Bind();
}

void WebGPUVertexBuffer::UnBind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//------------------------------------------------------------------------------------

WebGPUIndexBuffer::WebGPUIndexBuffer(uint32_t* indices, uint32_t count)
	:m_Count(count)
{
	PROFILE_FUNCTION();
    glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

WebGPUIndexBuffer::~WebGPUIndexBuffer()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteBuffers(1, &m_RendererID);
}

void WebGPUIndexBuffer::Bind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void WebGPUIndexBuffer::UnBind() const
{
	PROFILE_FUNCTION();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}