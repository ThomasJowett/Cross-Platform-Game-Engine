#pragma once
#include "Renderer/FrameBuffer.h"
class OpenGLFrameBuffer :
    public FrameBuffer
{
public:
    virtual void Bind() const override;
    virtual void UnBind() const override;

    virtual void Generate(uint32_t width, uint32_t height) override;
    virtual void Destroy() override;

    virtual void SetTextureSize(uint32_t width, uint32_t height) override;

    virtual uint32_t GetTextureID() override;
private:
    uint32_t m_FrameBufferObject;
    uint32_t m_Texture;
};

