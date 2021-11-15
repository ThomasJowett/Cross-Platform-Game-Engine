#pragma once

enum class FrameBufferTextureFormat
{
	None = 0,

	// Colour
	RGBA8,
	RED_INTEGER,

	//Depth/stencil
	DEPTH24STENCIL8,

	Depth = DEPTH24STENCIL8
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct FrameBufferTextureSpecification
{
	FrameBufferTextureSpecification() = default;
	FrameBufferTextureSpecification(FrameBufferTextureFormat format)
		:textureFormat(format) {}

	FrameBufferTextureFormat textureFormat;
	// TODO: filtering/wrap
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct FrameBufferAttachmentSpecification
{
	FrameBufferAttachmentSpecification() = default;
	FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
		:attachments(attachments) {}

	std::vector<FrameBufferTextureSpecification> attachments;
};

/* ------------------------------------------------------------------------------------------------------------------ */

struct FrameBufferSpecification
{
	uint32_t width, height;
	FrameBufferAttachmentSpecification attachments;
	uint32_t samples = 1;

	bool swapChainTarget = false;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class FrameBuffer
{
public:
	virtual ~FrameBuffer() = default;

	virtual void Bind() = 0;
	virtual void UnBind() = 0;

	virtual void Generate() = 0;
	virtual void Destroy() = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;

	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

	virtual uint32_t GetColourAttachment(size_t index = 0) = 0;

	virtual const FrameBufferSpecification& GetSpecification() const = 0;

	virtual void ClearAttachment(size_t index, int value) = 0;

	static Ref<FrameBuffer>Create(const FrameBufferSpecification& specification);

	static bool IsDepthFormat(FrameBufferTextureFormat format);
};