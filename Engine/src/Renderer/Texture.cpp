#include "stdafx.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

Ref<Texture2D> Texture2D::Create(const std::string & path)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(path);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		CORE_ASSERT(false, "Could not create Vertex Array: DirectX is not currently supported")
			return nullptr;
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
