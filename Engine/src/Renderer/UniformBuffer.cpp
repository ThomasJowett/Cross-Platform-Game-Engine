#include "stdafx.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGlUniformBuffer.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11UniformBuffer.h"
#endif // _WINDOWS

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLUniformBuffer>(size, binding);
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11UniformBuffer>(size, binding);
#endif // _WINDOWS
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Uniform Buffer: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Uniform Buffer: Invalid Renderer API")
		return nullptr;
}
