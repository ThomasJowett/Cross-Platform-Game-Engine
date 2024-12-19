#include "stdafx.h"
#include "FrameBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"
#include "Platform/WebGPU/WebGPUFrameBuffer.h"

Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& specification)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLFrameBuffer>(specification);
	case RendererAPI::API::WebGPU:
		return CreateRef<WebGPUFrameBuffer>(specification);
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