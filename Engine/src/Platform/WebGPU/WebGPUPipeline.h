#pragma once

#include "Renderer/Pipeline.h"
#include "WebGPUContext.h"
#include <webgpu/webgpu.hpp>

class WebGPUPipeline : public Pipeline
{
public:
	WebGPUPipeline(const Spec& spec);
	~WebGPUPipeline();

	// Inherited via Pipeline
	virtual void Invalidate() override;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) override;
	virtual void Bind() override;
private:
	bool m_TransparencyEnabled = false;
	//uint32_t m_VertexArray = -1;
	BufferLayout m_VertexBufferLayout;
	bool m_BackfaceCull = true;

	Ref<Shader> m_Shader = nullptr;

	wgpu::RenderPipeline m_Pipeline;

	Ref<WebGPUContext> m_WebGPUContext;
};
