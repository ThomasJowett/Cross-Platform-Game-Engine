#include "stdafx.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

VertexArray * VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::None:
		break;
	case RendererAPI::OpenGL:
		return new OpenGLVertexArray();
#ifdef __WINDOWS__
	case RendererAPI::Directx11:
		CORE_ASSERT(false, "Could not create Vertex Array: DirectX is not currently supported")
			return nullptr;
#endif // _Windows_
#ifdef __APPLE__
	case RendererAPI::Metal:
		CORE_ASSERT(false, "Could not create Vertex Array: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::Vulkan:
		CORE_ASSERT(false, "Could not create Vertex Array: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Vertex Array: Invalid Renderer API")
		return nullptr;
}
