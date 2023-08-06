#include "stdafx.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGlUniformBuffer.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11UniformBuffer.h"
#endif // __WINDOWS__
#include "Platform/Vulkan/VulkanUniformBuffer.h"

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLUniformBuffer>(size, binding);
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		return CreateRef<DirectX11UniformBuffer>(size, binding);
#endif // __WINDOWS__
#ifdef __APPLE__
	case RendererAPI::API::Metal:
		CORE_ASSERT(false, "Could not create Uniform Buffer: Metal is not currently supported");
		return nullptr;
#endif // __APPLE__
	case RendererAPI::API::Vulkan:
		return CreateRef<VulkanUniformBuffer>(size, binding);
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Uniform Buffer: Invalid Renderer API")
		return nullptr;
}
