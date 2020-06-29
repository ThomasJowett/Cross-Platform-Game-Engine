#pragma once
#include "Renderer/FrameBuffer.h"
class OpenGLFrameBuffer :
    public FrameBuffer
{
public:
    OpenGLFrameBuffer(const FrameBufferSpecification& specification);
    virtual ~OpenGLFrameBuffer();
    virtual void Bind() override;
    virtual void UnBind() override;

    virtual void Generate() override;
    virtual void Destroy() override;

    virtual void Resize(uint32_t width, uint32_t height) override;

    virtual uint32_t GetColourAttachment() override;

    virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }
private:
    uint32_t m_RendererID;
    uint32_t m_ColourAttachment;
    uint32_t m_DepthAttachment;

    FrameBufferSpecification m_Specification;
};