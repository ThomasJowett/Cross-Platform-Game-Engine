#pragma once
#include "Renderer/FrameBuffer.h"
class DirectX11FrameBuffer :
    public FrameBuffer
{
public:
    DirectX11FrameBuffer(const FrameBufferSpecification& specification);
    ~DirectX11FrameBuffer();

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

    virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification;  }

    virtual void ClearAttachment(size_t index, int value) override;
private:

    FrameBufferSpecification m_Specification;
};

