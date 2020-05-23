#include "stdafx.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11VertexArray.h"
#endif // __WINDOWS__


Ref<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLVertexArray>();
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create Vertex Array: DirectX is not currently supported");
			return	CreateRef<DirectX11VertexArray>();
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Vertex Array: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::API::Vulkan:
		CORE_ASSERT(false, "Could not create Vertex Array: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Vertex Array: Invalid Renderer API")
		return nullptr;
}
