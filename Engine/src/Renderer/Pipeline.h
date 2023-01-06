#pragma once
#include "Shader.h"
#include "Buffer.h"
#include "UniformBuffer.h"

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

class Pipeline
{
public:
	struct Spec
	{
		Ref<Shader> shader;
		BufferLayout layout;
		bool backFaceCulling = true;
		bool depthTest = true;
		bool transparencyEnabled = false;
	};
public:
	virtual ~Pipeline() = default;

	virtual Spec GetSpecification() { return m_Specification; }
	virtual const Spec& GetSpecification() const { return m_Specification; }

	virtual void Invalidate() = 0;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set = 0) = 0;

	virtual void Bind() = 0;

	static Ref<Pipeline> Create(const Spec& spec);

protected:
	Spec m_Specification;
};
