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

    virtual uint32_t GetColourAttachment() override;

    virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification;  }
private:

    FrameBufferSpecification m_Specification;
};

