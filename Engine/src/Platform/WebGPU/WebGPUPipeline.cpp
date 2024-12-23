#include "stdafx.h"
#include "WebGPUPipeline.h"

WebGPUPipeline::WebGPUPipeline(const Spec& spec)
{
	m_TransparencyEnabled = spec.transparencyEnabled;
	m_BackfaceCull = spec.backFaceCulling;
	m_Specification = spec;

	wgpu::RenderPipelineDescriptor pipelineDesc;

	wgpu::VertexBufferLayout vertexBufferLayout;
	//TODO: configure the layout from m_Specification

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);
	auto device = m_WebGPUContext->GetWebGPUDevice();


	m_Pipeline = device.createRenderPipeline(pipelineDesc);
}

WebGPUPipeline::~WebGPUPipeline()
{
	m_Pipeline.release();
}

void WebGPUPipeline::Invalidate()
{
}

void WebGPUPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
{
}

void WebGPUPipeline::Bind()
{
	//renderPass.setPipeline(m_Pipeline);
}