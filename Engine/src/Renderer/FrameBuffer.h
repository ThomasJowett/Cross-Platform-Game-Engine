#pragma once
class FrameBuffer
{
public:
	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	virtual void Generate(uint32_t width, uint32_t  height) = 0;
	virtual void Destroy() = 0;

	virtual void SetTextureSize(uint32_t width, uint32_t height) = 0;

	virtual uint32_t GetTextureID() = 0;

	static Ref<FrameBuffer>Create();
};

