#include "stdafx.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#ifdef __WINDOWS__
#include "Platform/DirectX/DirectX11FrameBuffer.h"
#endif // __WINDOWS__

Ref<FrameBuffer> FrameBuffer::Create()
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::Directx11:
		ENGINE_WARN("Could not create Frame Buffer: DirectX is not currently supported");
		return CreateRef<DirectX11FrameBuffer>();
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLFrameBuffer>();
	default:
		break;
	}
	return Ref<FrameBuffer>();
}
