#include "stdafx.h"
#include "DirectX11FrameBuffer.h"

DirectX11FrameBuffer::DirectX11FrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification)
{
}

DirectX11FrameBuffer::~DirectX11FrameBuffer()
{
	Destroy();
}

void DirectX11FrameBuffer::Bind()
{
	//TODO: directX frame buffer bind
}

void DirectX11FrameBuffer::UnBind()
{
	//TODO: directX frame buffer unbind
}

void DirectX11FrameBuffer::Generate()
{
	//TODO: directx framebuffer generate
}

void DirectX11FrameBuffer::Destroy()
{
	//TODO: directx frame buffer destroy
}

void DirectX11FrameBuffer::SetTextureSize(uint32_t width, uint32_t height)
{
}

uint32_t DirectX11FrameBuffer::GetColourAttachment()
{
	return 0;
}
