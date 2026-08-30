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
		PrimitiveTopology topology = PrimitiveTopology::Triangles;
		uint32_t samples = 1;
		// No colour targets - fragment shader only writes @builtin(frag_depth), always (depthTest ignored).
		bool depthOnly = false;
	};
public:
	virtual ~Pipeline() = default;

	virtual Spec GetSpecification() { return m_Specification; }
	virtual const Spec& GetSpecification() const { return m_Specification; }

	virtual void Invalidate() = 0;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set = 0) = 0;
	virtual void SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set = 0) = 0;

	// Binds textures[i] at firstBinding+i, for shaders that select among several textures
	// per-fragment (e.g. multiple font atlases). All textures must use equivalent sampler
	// settings - samplerSource supplies them for backends that bind one shared sampler.
	// TODO(texture-array-cleanup): "N separate bindings" is a stand-in for a real array texture -
	// see WebGPUPipeline::SetTextureArray.
	virtual void SetTextureArray(const std::vector<Ref<Texture>>& textures, uint32_t firstBinding, Ref<Texture> samplerSource, uint32_t set = 0) = 0;

	virtual void Bind() = 0;

	// False if the underlying graphics-API pipeline object failed to build (e.g. its shader
	// couldn't be loaded) - callers must skip drawing rather than binding buffers and issuing
	// a draw call against no bound pipeline, which some backends treat as a fatal error.
	virtual bool IsValid() const = 0;

	static Ref<Pipeline> Create(const Spec& spec);

protected:
	Spec m_Specification;
};
