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

void DirectX11FrameBuffer::Resize(uint32_t width, uint32_t height)
{
}

int DirectX11FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	return 0;
}

Ref<Texture> DirectX11FrameBuffer::GetColourAttachment(size_t index)
{
	return 0;
}

Ref<Texture> DirectX11FrameBuffer::GetDepthAttachment()
{
	return 0;
}

void DirectX11FrameBuffer::BlitDepthTo(Ref<FrameBuffer> target)
{
}

void DirectX11FrameBuffer::BlitColourTo(Ref<FrameBuffer> target, uint32_t srcAttachmentIndex, uint32_t dstAttachmentIndex)
{
}

void DirectX11FrameBuffer::ClearAttachment(size_t index, int value)
{
}
