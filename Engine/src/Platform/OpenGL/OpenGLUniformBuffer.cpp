#include "OpenGLUniformBuffer.h"
#include "Core/Application.h"

#include <glad/glad.h>

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	:m_Binding(binding)
{
	glCreateBuffers(1, &m_RendererID);
	glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
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

void OpenGLUniformBuffer::Bind() const
{
	// GL_UNIFORM_BUFFER binding points are global GL state, not per-buffer - binding this once
	// at construction (the old behaviour) meant a later-constructed buffer's constructor would
	// silently steal the binding point out from under an earlier one (e.g. the model-UBO pool
	// growing mid-frame), leaving every draw that used an earlier pool slot reading whichever
	// buffer happens to still be bound instead of the one it actually wrote its data into. Must
	// be re-issued on every bind, not just once.
	glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
}
