#include "stdafx.h"
#include "Buffer.h"

#include "Core/core.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/WebGPU/WebGPUBuffer.h"

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		CORE_ASSERT(false, "Could not create vertex buffer: None Renderer API is not supported")
			return nullptr;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLVertexBuffer>(size);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUVertexBuffer>(size);
	default:
		break;
	}

	CORE_ASSERT(false, "Could not create vertex buffer: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		CORE_ASSERT(false, "Could not create vertex buffer: None Renderer API is not supported")
			return nullptr;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLVertexBuffer>(vertices, size);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUVertexBuffer>(vertices, size);
	default:
		break;
	}

	CORE_ASSERT(false, "Could not create vertex buffer: Invalid Renderer API")
		return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLIndexBuffer>(indices, size);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUIndexBuffer>(indices, size);
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create index buffer: Invalid Renderer API")
		return nullptr;
}