#include "WebGPUPipeline.h"
#include "Core/Application.h"
#include "Renderer/Buffer.h"
#include "Renderer/RenderCommand.h"
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

WebGPUPipeline::WebGPUPipeline(const Spec& spec) { Invalidate(); }

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
	wgpu::DepthStencilState depthStencil = {};
	if (m_Specification.hasDepth)
	{
		ENGINE_TRACE("WebGPUPipeline: Creating pipeline WITH depth stencil for shader: {0}", webGPUShader->GetName());
		depthStencil.format = wgpu::TextureFormat::Depth24PlusStencil8;
		depthStencil.depthWriteEnabled = m_Specification.depthTest;
		depthStencil.depthCompare = m_Specification.depthTest ? wgpu::CompareFunction::Less : wgpu::CompareFunction::Always;
		depthStencil.stencilFront.compare = wgpu::CompareFunction::Always;
		depthStencil.stencilBack.compare = wgpu::CompareFunction::Always;

		pipelineDesc.depthStencil = &depthStencil;
	}

	// Multisample state
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = 0xFFFFFFFF;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;
	pipelineDesc.layout = nullptr;

	m_Pipeline = device.createRenderPipeline(pipelineDesc);
void WebGPUPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
{
	auto& bindings = m_Bindings[set];
	bool found = false;
	for (auto& b : bindings)
	{
		if (b.binding == binding)
		{
			b.resource = uniformBuffer;
			b.type = Binding::Type::UniformBuffer;
			found = true;
			break;
		}
	}
	if (!found)
		bindings.push_back({ Binding::Type::UniformBuffer, binding, uniformBuffer });
}

void WebGPUPipeline::SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set)
{
	auto& bindings = m_Bindings[set];
	bool found = false;
	for (auto& b : bindings)
	{
		if (b.binding == binding)
		{
			b.resource = texture;
			b.type = Binding::Type::Texture;
			found = true;
			break;
		}
	}
	if (!found)
		bindings.push_back({ Binding::Type::Texture, binding, texture });
}

void WebGPUPipeline::Bind()
{
	auto& rendererAPI = static_cast<WebGPURendererAPI&>(RenderCommand::Get());
	rendererAPI.GetRenderPass().setPipeline(m_Pipeline);
void WebGPUPipeline::CommitBindGroups()
{
	auto& rendererAPI = static_cast<WebGPURendererAPI&>(RenderCommand::Get());
	auto device = m_WebGPUContext->GetWebGPUDevice();

	for (auto& [set, bindings] : m_Bindings)
	{
		std::vector<wgpu::BindGroupEntry> entries;
		for (auto& b : bindings)
		{
			wgpu::BindGroupEntry entry = {};
			entry.binding = b.binding;
			if (b.type == Binding::Type::UniformBuffer)
			{
				auto ub = std::static_pointer_cast<WebGPUUniformBuffer>(std::static_pointer_cast<UniformBuffer>(b.resource));
				if (ub)
				{
					entry.buffer = ub->GetBuffer();
					entry.offset = 0;
					entry.size = ub->GetSize();
					entries.push_back(entry);
				}
			}
			else if (b.type == Binding::Type::Texture)
			{
				auto tex = std::static_pointer_cast<WebGPUTexture2D>(std::static_pointer_cast<Texture>(b.resource));
				if (tex)
				{
					wgpu::BindGroupEntry entry = {};
					entry.binding = b.binding;
					entry.textureView = tex->GetTextureView();
					entries.push_back(entry);

					if (b.binding <= 1)
					{
						wgpu::BindGroupEntry samplerEntry = {};
						samplerEntry.binding = b.binding + 1;
						samplerEntry.sampler = tex->GetSampler();
						if (samplerEntry.sampler)
							entries.push_back(samplerEntry);
					}
				}
			}
		}

		if (entries.empty())
			continue;

		wgpu::BindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = m_Pipeline.getBindGroupLayout(set);
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = entries.data();

		wgpu::BindGroup bindGroup = device.createBindGroup(bindGroupDesc);
		rendererAPI.GetRenderPass().setBindGroup(set, bindGroup, 0, nullptr);
	}
}