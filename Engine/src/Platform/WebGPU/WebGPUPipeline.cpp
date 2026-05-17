#include "WebGPUPipeline.h"
#include "Core/Application.h"
#include "Renderer/Buffer.h"
#include "WebGPURendererAPI.h"
#include "WebGPUShader.h"
#include <memory>
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

WebGPUPipeline::~WebGPUPipeline()
{
	if (m_Pipeline)
		m_Pipeline.release();
}

void WebGPUPipeline::Invalidate()
{
	if (m_Pipeline)
		m_Pipeline.release();

	auto device = m_WebGPUContext->GetWebGPUDevice();

	Ref<WebGPUShader> webGPUShader = std::dynamic_pointer_cast<WebGPUShader>(m_Specification.shader);
	if (!webGPUShader)
	{
		ENGINE_ERROR("WebGPUPipeline: Shader is null!");
		return;
	}

	wgpu::ShaderModule shaderModule = webGPUShader->GetShaderModule();
	if (!shaderModule)
	{
		ENGINE_ERROR("WebGPUPipeline: Shader module is null!");
	}

	wgpu::RenderPipelineDescriptor pipelineDesc;

	// Vertex state
	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = "vs_main";

	// Vertex attributes
	std::vector<wgpu::VertexAttribute> vertexAttributes;
	const auto& layout = m_Specification.layout;
	uint32_t location = 0;
	for (const auto& element : layout)
	{
		wgpu::VertexAttribute attr;
		attr.shaderLocation = location++;
		attr.format = ShaderDataTypeToWebGPU(element.type);
		attr.offset = element.offset;
		vertexAttributes.push_back(attr);
	}

	wgpu::FragmentState fragmentState;
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";

	// Colour target state
	wgpu::ColorTargetState colourTarget;
	colourTarget.format = m_WebGPUContext->GetSwapchainFormat();
	colourTarget.writeMask = wgpu::ColorWriteMask::All;

	wgpu::BlendState blend;
	if (m_Specification.transparencyEnabled)
	{
		blend.color.operation = wgpu::BlendOperation::Add;
		blend.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
		blend.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		blend.alpha.operation = wgpu::BlendOperation::Add;
		blend.alpha.srcFactor = wgpu::BlendFactor::One;
		blend.alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		colourTarget.blend = &blend;
	}

	fragmentState.targetCount = 1;
	fragmentState.targets = &colourTarget;
	pipelineDesc.fragment = &fragmentState;

	// Primitive state
	pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
	if (m_Specification.backFaceCulling)
	{
		pipelineDesc.primitive.cullMode = wgpu::CullMode::Back;
	}
	else
	{
		pipelineDesc.primitive.cullMode = wgpu::CullMode::None;
	}

	// Depth stencil state
	wgpu::DepthStencilState depthStencil;
	if (m_Specification.depthTest)
	{
		depthStencil.format = wgpu::TextureFormat::Depth24Plus;
		depthStencil.depthWriteEnabled = true;
		depthStencil.depthCompare = wgpu::CompareFunction::Less;
		pipelineDesc.depthStencil = &depthStencil;
	}

	// Multisample state
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = 0xFFFFFFFF;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;
	pipelineDesc.layout = nullptr;

	m_Pipeline = device.createRenderPipeline(pipelineDesc);
}

void WebGPUPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) {}

void WebGPUPipeline::Bind()
{
	// renderPass.setPipeline(m_Pipeline);
}