#include "WebGPUPipeline.h"
#include "Renderer/Buffer.h"
#include <webgpu/webgpu.hpp>

static wgpu::VertexFormat ShaderDataTypeToWebGPU(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
		return wgpu::VertexFormat::Float32;
	case ShaderDataType::Float2:
		return wgpu::VertexFormat::Float32x2;
	case ShaderDataType::Float3:
		return wgpu::VertexFormat::Float32x3;
	case ShaderDataType::Float4:
		return wgpu::VertexFormat::Float32x4;
	case ShaderDataType::Int:
		return wgpu::VertexFormat::Sint32;
	case ShaderDataType::Int2:
		return wgpu::VertexFormat::Sint32x2;
	case ShaderDataType::Int3:
		return wgpu::VertexFormat::Sint32x3;
	case ShaderDataType::Int4:
		return wgpu::VertexFormat::Sint32x4;
	case ShaderDataType::Mat3:
		return wgpu::VertexFormat::Float32x3;
	case ShaderDataType::Mat4:
		return wgpu::VertexFormat::Float32x4;
	case ShaderDataType::Bool:
		return wgpu::VertexFormat::Uint32;
	case ShaderDataType::None:
		break;
	}
	return wgpu::VertexFormat::Undefined;
}

WebGPUPipeline::WebGPUPipeline(const Spec& spec)
{
	m_TransparencyEnabled = spec.transparencyEnabled;
	m_BackfaceCull = spec.backFaceCulling;
	m_Specification = spec;

	wgpu::RenderPipelineDescriptor pipelineDesc;

	wgpu::VertexBufferLayout vertexBufferLayout;
	// TODO: configure the layout from m_Specification

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();
	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);
	auto device = m_WebGPUContext->GetWebGPUDevice();


	m_Pipeline = device.createRenderPipeline(pipelineDesc);
}

WebGPUPipeline::~WebGPUPipeline() { m_Pipeline.release(); }

void WebGPUPipeline::Invalidate() {}

void WebGPUPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) {}

void WebGPUPipeline::Bind()
{
	// renderPass.setPipeline(m_Pipeline);
}