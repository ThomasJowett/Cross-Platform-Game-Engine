#pragma once
#include "Renderer/FrameBuffer.h"
#include "WebGPUTexture.h"

#include <webgpu/webgpu.hpp>
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

	virtual Ref<Texture> GetColourAttachment(size_t index) override;
	virtual Ref<Texture> GetDepthAttachment() override;

	virtual void BlitDepthTo(Ref<FrameBuffer> target) override;
	virtual void BlitColourTo(Ref<FrameBuffer> target, uint32_t srcAttachmentIndex, uint32_t dstAttachmentIndex) override;

	virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	virtual void ClearAttachment(size_t index, int value) override;
	static WebGPUFrameBuffer* GetCurrent();
private:
	FrameBufferSpecification m_Specification;

	std::vector<Ref<WebGPUTexture2D>> m_ColourAttachments;
	Ref<WebGPUTexture2D> m_DepthAttachment;

	std::vector<wgpu::TextureView> m_ColourViews;
	wgpu::TextureView m_DepthView;

	wgpu::Device m_Device;

	static WebGPUFrameBuffer* s_Current;
};