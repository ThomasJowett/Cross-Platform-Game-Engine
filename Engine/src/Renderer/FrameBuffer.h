#pragma once

struct FrameBufferSpecification
{
	uint32_t Width, Height;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class FrameBuffer
{
public:
	virtual ~FrameBuffer() = default;

	virtual void Bind() = 0;
	virtual void UnBind() = 0;

	virtual void Generate() = 0;
	virtual void Destroy() = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual uint32_t GetColourAttachment() = 0;

	virtual const FrameBufferSpecification& GetSpecification() const = 0;

	static Ref<FrameBuffer>Create(const FrameBufferSpecification& specification);
};