#include "stdafx.h"
#include "VulkanFrameBuffer.h"

#include "Logging/Instrumentor.h"

VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification), m_RendererID(0)
{
	for (auto spec : m_Specification.attachments.attachments)
	{
		if (!IsDepthFormat(spec.textureFormat))
		{
			m_ColourAttachmentSpecifications.emplace_back(spec);
		}
		else
			m_DepthAttachmentSpecification = spec;
	}

	Generate();
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	Destroy();
}

void VulkanFrameBuffer::Bind()
{
	PROFILE_FUNCTION();


}

void VulkanFrameBuffer::UnBind()
{
	PROFILE_FUNCTION();
}

void VulkanFrameBuffer::Generate()
{
	PROFILE_FUNCTION();

	if (m_RendererID)
		Destroy();

}

void VulkanFrameBuffer::Destroy()
{
}

void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
{
}

int VulkanFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	return 0;
}

uint32_t VulkanFrameBuffer::GetColourAttachment(size_t index)
{
	return 0;
}

const FrameBufferSpecification& VulkanFrameBuffer::GetSpecification() const
{
	return m_Specification;
}

void VulkanFrameBuffer::ClearAttachment(size_t index, int value)
{
}
