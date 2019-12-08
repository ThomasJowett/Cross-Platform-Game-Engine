#include "stdafx.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

Shader * Shader::Create(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return new OpenGLShader(vertexShaderSrc, fragmentShaderSrc);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		CORE_ASSERT(false, "Could not create Shader: DirectX is not currently supported")
			return nullptr;
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Shader: Metal is not currently supported")
			return nullptr;
#endif // __APPLE__
	case RendererAPI::API::Vulkan:
		CORE_ASSERT(false, "Could not create Shader: Vulkan is not currently supported")
			return nullptr;
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Shader: Invalid Renderer API")
		return nullptr;
}