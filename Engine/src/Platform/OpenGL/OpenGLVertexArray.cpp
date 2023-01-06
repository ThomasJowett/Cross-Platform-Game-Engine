#include "stdafx.h"
#include "OpenGLVertexArray.h"
#include "Renderer/Buffer.h"
#include "Core/Application.h"

#include <glad/glad.h>

// Temporary function
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:		return GL_FLOAT;
	case ShaderDataType::Float2:	return GL_FLOAT;
	case ShaderDataType::Float3:	return GL_FLOAT;
	case ShaderDataType::Float4:	return GL_FLOAT;
	case ShaderDataType::Mat3:		return GL_FLOAT;
	case ShaderDataType::Mat4:		return GL_FLOAT;
	case ShaderDataType::Int:		return GL_INT;
	case ShaderDataType::Int2:		return GL_INT;
	case ShaderDataType::Int3:		return GL_INT;
	case ShaderDataType::Int4:		return GL_INT;
	case ShaderDataType::Bool:		return GL_BOOL;
	default:
		break;
	}

	CORE_ASSERT(false, "Shader Data type not recognised");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	PROFILE_FUNCTION();
	glGenVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning())
		glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	PROFILE_FUNCTION();
	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::UnBind() const
{
	PROFILE_FUNCTION();
	glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const VertexBuffer* vertexBuffer)
{
	PROFILE_FUNCTION();
	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();

	CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout has no elements");

	const BufferLayout& layout = vertexBuffer->GetLayout();
	for (const BufferElement& element : layout)
	{
		switch (element.type)
		{
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.Count(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
		case ShaderDataType::Bool:
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribIPointer(m_VertexBufferIndex,
				element.Count(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				layout.GetStride(),
				(const void*)element.offset);
			m_VertexBufferIndex++;
			break;
		}
		case ShaderDataType::Mat3:
		case ShaderDataType::Mat4:
		{
			uint8_t count = element.Count();
			for (uint8_t i = 0; i < count; i++)
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					count,
					ShaderDataTypeToOpenGLBaseType(element.type),
					element.normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)(sizeof(float) * count * i));
				glVertexAttribDivisor(m_VertexBufferIndex, 1);
				m_VertexBufferIndex++;
			}
			break;
		}
		default:
			CORE_ASSERT(false, "Unknown ShaderDataType");
		}
	}

	//m_VertexBuffers.push_back(vertexBuffer);
}

//void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
//{
//	PROFILE_FUNCTION();
//	glBindVertexArray(m_RendererID);
//	indexBuffer->Bind();
//
//	m_IndexBuffer = indexBuffer;
//}

//const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
//{
//	return m_VertexBuffers;
//}

//const Ref<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
//{
//	return m_IndexBuffer;
//}