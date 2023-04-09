#pragma once

#include "Renderer/FrameBuffer.h"

class VulkanFrameBuffer : public FrameBuffer
{
public:
	VulkanFrameBuffer(const FrameBufferSpecification& spec);
	virtual ~VulkanFrameBuffer();

	// Inherited via FrameBuffer
	virtual void Bind() override;
	virtual void UnBind() override;
	virtual void Generate() override;
	virtual void Destroy() override;
	virtual void Resize(uint32_t width, uint32_t height) override;
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
	virtual uint32_t GetColourAttachment(size_t index) override;
	virtual const FrameBufferSpecification& GetSpecification() const override;
	virtual void ClearAttachment(size_t index, int value) override;

private:
	uint32_t m_RendererID;

	FrameBufferSpecification m_Specification;
	std::vector<FrameBufferTextureSpecification> m_ColourAttachmentSpecifications;
	FrameBufferTextureSpecification m_DepthAttachmentSpecification;
};
