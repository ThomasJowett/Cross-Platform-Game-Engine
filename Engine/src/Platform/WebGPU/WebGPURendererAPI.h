#pragma once

#include "Renderer/RendererAPI.h"
#include "WebGPUContext.h"
#include "Renderer/Pipeline.h"
#include <webgpu/webgpu.hpp>

class WebGPURendererAPI : public RendererAPI
{
public:
	virtual bool Init() override;
	virtual void SetClearColour(const Colour& colour) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void Clear() override;
	virtual void ClearColour() override;
	virtual void ClearDepth() override;

	virtual void StartRenderPass(bool clear = true) override;
	virtual void EndRenderPass() override;

	virtual void DrawIndexed(uint32_t indexCount, uint32_t indexStart = 0, uint32_t vertexOffset = 0, DrawMode drawMode = DrawMode::FILL) override;
	virtual void DrawLines(uint32_t vertexCount) override;

	void SetCurrentPipeline(Ref<Pipeline> pipeline) { m_CurrentPipeline = pipeline; }
	wgpu::RenderPassEncoder GetRenderPass();

private:
	Colour m_ClearColour = Colours::NAVY;
	wgpu::RenderPassEncoder m_RenderPass;
	wgpu::CommandEncoder m_CommandEncoder;

	uint32_t m_CurrentTargetWidth = 0;
	uint32_t m_CurrentTargetHeight = 0;

	Ref<Pipeline> m_CurrentPipeline;
	Ref<WebGPUContext> m_WebGPUContext;
};