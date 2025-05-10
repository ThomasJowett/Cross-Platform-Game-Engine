#include "stdafx.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#ifdef _WINDOWS
#include "Platform/DirectX/DirectX11FrameBuffer.h"
#endif // _WINDOWS

Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& specification)
{
	switch (Renderer::GetAPI())
	{
#ifdef _WINDOWS
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create Frame Buffer: DirectX is not currently supported");
		return CreateRef<DirectX11FrameBuffer>(specification);
#endif // _WINDOWS
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLFrameBuffer>(specification);
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