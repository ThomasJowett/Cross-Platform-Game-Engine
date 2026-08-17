#pragma once

#include "Renderer/Pipeline.h"
#include "OpenGLVertexArray.h"

class OpenGLPipeline : public Pipeline
{
public:
	OpenGLPipeline(const Spec& spec);
	~OpenGLPipeline();

	// Inherited via Pipeline
	virtual void Invalidate() override;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) override;
	virtual void SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set) override;
	virtual void Bind() override;
	virtual bool IsValid() const override { return true; }

	// Configures this pipeline's VAO to read from vertexBuffer using this pipeline's own
	// Spec::layout (not whatever layout the buffer itself was last told) - called from
	// OpenGLVertexBuffer::Bind() via the currently-bound pipeline, so every draw site's
	// existing pipeline->Bind(); vertexBuffer->Bind(); sequence keeps working unchanged.
	void ConfigureVertexBuffer(const VertexBuffer* vertexBuffer);

private:
	bool m_TransparencyEnabled = false;
	bool m_BackfaceCull = true;

	Ref<OpenGLVertexArray> m_VertexArray;
	const VertexBuffer* m_LastConfiguredVertexBuffer = nullptr;
};
