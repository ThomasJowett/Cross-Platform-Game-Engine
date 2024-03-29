#include "stdafx.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11FrameBuffer.h"
#endif // __WINDOWS__
#include "Platform/Vulkan/VulkanFrameBuffer.h"

Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& specification)
{
	switch (Renderer::GetAPI())
	{
#ifdef __WINDOWS__
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create Frame Buffer: DirectX is not currently supported");
		return CreateRef<DirectX11FrameBuffer>(specification);
#endif // __WINDOWS__
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLFrameBuffer>(specification);
	case RendererAPI::API::Vulkan:
		return CreateRef<VulkanFrameBuffer>(specification);
	default:
		break;
	}
	return Ref<FrameBuffer>();
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool FrameBuffer::IsDepthFormat(FrameBufferTextureFormat format)
{
	switch (format)
	{
	case FrameBufferTextureFormat::DEPTH24STENCIL8:
		return true;
	default:
		break;
	}
	return false;
}