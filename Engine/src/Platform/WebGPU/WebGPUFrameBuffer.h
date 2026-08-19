#pragma once
#include "Renderer/FrameBuffer.h"
#include "WebGPUTexture.h"

#include <webgpu/webgpu.hpp>

class Pipeline;
class Mesh;

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

	virtual void SetResolveTarget(Ref<FrameBuffer> target) override;
	virtual void ResolveTo(Ref<FrameBuffer> target) override;

	virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

	virtual void ClearAttachment(size_t index, int value) override;

	const std::vector<wgpu::TextureView>& GetColourViews() const { return m_ColourViews; }
	wgpu::TextureView GetDepthView() const { return m_DepthView; }

	// Read by WebGPURendererAPI::StartRenderPass to fill in each non-integer colour attachment's
	// resolveTarget - WebGPU resolves multisampled colour as part of the render pass itself, not
	// as a separate operation afterward.
	Ref<FrameBuffer> GetResolveTarget() const { return m_ResolveTarget; }

	static WebGPUFrameBuffer* GetCurrent();
private:
	FrameBufferSpecification m_Specification;

	std::vector<Ref<WebGPUTexture2D>> m_ColourAttachments;
	Ref<WebGPUTexture2D> m_DepthAttachment;

	std::vector<wgpu::TextureView> m_ColourViews;
	wgpu::TextureView m_DepthView;

	wgpu::Device m_Device;

	Ref<FrameBuffer> m_ResolveTarget;

	// Lazily built the first time ResolveTo() actually needs to resolve an integer attachment.
	Ref<Pipeline> m_EntityIdResolvePipeline;
	Ref<Mesh> m_ResolveFullscreenQuad;

	// Lazily built the first time BlitDepthTo() actually needs to copy a depth buffer.
	Ref<Pipeline> m_DepthBlitPipeline;
	Ref<Mesh> m_DepthBlitFullscreenQuad;

	static WebGPUFrameBuffer* s_Current;
};