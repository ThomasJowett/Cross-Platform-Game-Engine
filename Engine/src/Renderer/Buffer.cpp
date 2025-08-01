#include "stdafx.h"
#include "Buffer.h"

#include "Core/core.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11Buffer.h"
#endif // _WINDOWS

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		CORE_ASSERT(false, "Could not create vertex buffer: None Renderer API is not supported")
			return nullptr;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLVertexBuffer>(size);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create vertex buffer: DirectX is not currently supported");
		return CreateRef<DirectX11VertexBuffer>(size);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create vertex buffer: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
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
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create vertex buffer: DirectX is not currently supported");
		return CreateRef<DirectX11VertexBuffer>(vertices, size);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create vertex buffer: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
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
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create index buffer: DirectX is not currently supported");
		return CreateRef<DirectX11IndexBuffer>(indices, size);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create index buffer: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create index buffer: Invalid Renderer API")
		return nullptr;
}