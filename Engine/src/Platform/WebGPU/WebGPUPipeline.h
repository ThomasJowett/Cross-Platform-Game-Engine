#pragma once

#include "Renderer/Pipeline.h"
#include "WebGPUContext.h"
#include <webgpu/webgpu.hpp>
#include <unordered_map>
#include <vector>

class WebGPUPipeline : public Pipeline
{
public:
	WebGPUPipeline(const Spec& spec);
	~WebGPUPipeline();

	// Inherited via Pipeline
	virtual void Invalidate() override;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) override;
	virtual void SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set) override;
	virtual void Bind() override;

	void CommitBindGroups();
private:
	struct Binding
	{
		enum class Type { UniformBuffer, Texture };
		Type type;
		uint32_t binding;
		Ref<void> resource;
	};
	std::unordered_map<uint32_t, std::vector<Binding>> m_Bindings; // set -> bindings

	bool m_TransparencyEnabled = false;
	//uint32_t m_VertexArray = -1;
	BufferLayout m_VertexBufferLayout;
	bool m_BackfaceCull = true;

	Ref<Shader> m_Shader = nullptr;

	wgpu::RenderPipeline m_Pipeline;

	Ref<WebGPUContext> m_WebGPUContext;
};
