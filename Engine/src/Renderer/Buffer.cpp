#include "stdafx.h"
#include "Buffer.h"

#include "Core/core.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

IndexBuffer * IndexBuffer::Create(uint32_t * indices, uint32_t size)
{
	switch (Renderer::GetAPI())	
	{
	case RendererAPI::None:
		break;
	case RendererAPI::OpenGL:
		return new OpenGLIndexBuffer(indices, size);
#ifdef __WINDOWS__
	case RendererAPI::Directx11:
		CORE_ASSERT(false, "Could not create index buffer: DirectX is not currently supported")
			return nullptr;
#endif // _Windows_
#ifdef __APPLE__
	case RendererAPI::Metal:
		CORE_ASSERT(false, "Could not create index buffer: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::Vulkan:
		CORE_ASSERT(false, "Could not create index buffer: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create index buffer: Invalid Renderer API")
	return nullptr;
}

VertexBuffer * VertexBuffer::Create(float * vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::None:
		CORE_ASSERT(false, "Could not create vertex buffer: None Renderer API is not supported")
		return nullptr;
	case RendererAPI::OpenGL:
		return new OpenGLVertexBuffer(vertices, size);
#ifdef __WINDOWS__
	case RendererAPI::Directx11:
		CORE_ASSERT(false, "Could not create vertex buffer: DirectX is not currently supported")
		return nullptr;
#endif // _Windows_
#ifdef __APPLE__
	case RendererAPI::Metal:
		CORE_ASSERT(false, "Could not create vertex buffer: Metal is not currently supported")
		return nullptr;
#endif // __APPLE__
	case RendererAPI::Vulkan:
		CORE_ASSERT(false, "Could not create vertex buffer: Vulkan is not currently supported")
		return nullptr;
	default:
		break;
	}

	CORE_ASSERT(false, "Could not create vertex buffer: Invalid Renderer API")
	return nullptr;
}
