#include "stdafx.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/WebGPU/WebGPUUniformBuffer.h"

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		break;
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLUniformBuffer>(size, binding);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUUniformBuffer>(size, binding);
	default:
		break;
	}

	CORE_ASSERT(true, "Could not create Uniform Buffer: Invalid Renderer API")
		return nullptr;
}
