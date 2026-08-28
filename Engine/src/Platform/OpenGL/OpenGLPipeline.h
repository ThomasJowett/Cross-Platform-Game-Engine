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
	virtual void SetTextureArray(const std::vector<Ref<Texture>>& textures, uint32_t firstBinding, Ref<Texture> samplerSource, uint32_t set) override;
	virtual void Bind() override;
	virtual bool IsValid() const override { return true; }

	void ConfigureVertexBuffer(const VertexBuffer* vertexBuffer);

private:
	Ref<OpenGLVertexArray> m_VertexArray;
	const VertexBuffer* m_LastConfiguredVertexBuffer = nullptr;
};
