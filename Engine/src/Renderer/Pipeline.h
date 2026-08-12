#pragma once
#include <memory>
#include "Asset/Shader.h"
#include "Buffer.h"
#include "UniformBuffer.h"

#include <vector>
#include "FrameBuffer.h"

enum class PrimitiveTopology
{
	None = 0,
	Points,
	Lines,
	Triangles,
	LineStrip,
	TriangleStrip,
	TriangleFan
};

class Pipeline : public std::enable_shared_from_this<Pipeline>
{
public:
	struct Spec
	{
		Ref<Shader> shader;
		BufferLayout layout;
		bool backFaceCulling = true;
		bool depthTest = true;
		bool transparencyEnabled = false;
		std::vector<FrameBufferTextureFormat> targetFormats;
		bool hasDepth = true;
	};
public:
	virtual ~Pipeline() = default;

	virtual Spec GetSpecification() { return m_Specification; }
	virtual const Spec& GetSpecification() const { return m_Specification; }

	virtual void Invalidate() = 0;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set = 0) = 0;
	virtual void SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set = 0) = 0;

	virtual void Bind() = 0;

	static Ref<Pipeline> Create(const Spec& spec);

protected:
	Spec m_Specification;
};
