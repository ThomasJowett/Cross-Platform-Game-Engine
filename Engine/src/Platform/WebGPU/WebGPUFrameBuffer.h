#pragma once
#include "Renderer/FrameBuffer.h"
class WebGPUFrameBuffer :
	public FrameBuffer
{
public:
	WebGPUFrameBuffer(const FrameBufferSpecification& specification);
	virtual ~WebGPUFrameBuffer();
	virtual void Bind() override;
	virtual void UnBind() override;

	virtual void Generate() override;
	virtual void Destroy() override;

	virtual void Resize(uint32_t width, uint32_t height) override;

	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

	virtual uint32_t GetColourAttachment(size_t index) override;

	virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	virtual void ClearAttachment(size_t index, int value) override;
private:

	uint32_t m_RendererID;
	std::vector<uint32_t> m_ColourAttachments;
	uint32_t m_DepthAttachment;

	FrameBufferSpecification m_Specification;
	std::vector<FrameBufferTextureSpecification> m_ColourAttachmentSpecifications;
	FrameBufferTextureSpecification m_DepthAttachmentSpecification;
};