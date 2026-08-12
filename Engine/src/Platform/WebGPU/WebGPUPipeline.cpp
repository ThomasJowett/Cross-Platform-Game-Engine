#include "WebGPUPipeline.h"
#include "Core/Application.h"
#include "Renderer/Buffer.h"
#include "Renderer/RenderCommand.h"
#include "WebGPURendererAPI.h"
#include "WebGPUShader.h"
#include "WebGPUUniformBuffer.h"
#include "WebGPUTexture.h"
#include <memory>
#include <webgpu/webgpu.hpp>

static wgpu::TextureFormat FrameBufferFormatToWebGPU(FrameBufferTextureFormat format)
{
	switch (format)
	{
	case FrameBufferTextureFormat::RGBA8:       return wgpu::TextureFormat::RGBA8Unorm;
	case FrameBufferTextureFormat::RED_INTEGER: return wgpu::TextureFormat::R32Sint;
	case FrameBufferTextureFormat::Depth:       return wgpu::TextureFormat::Depth24PlusStencil8;
	default:									return wgpu::TextureFormat::Undefined;
	}
}

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
	m_Specification = spec;
	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(Application::GetWindow()->GetContext());
	Invalidate();
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
		ENGINE_ERROR("WebGPUPipeline: Shader module is null for shader: {0}", webGPUShader->GetName());
		return;
	}

	wgpu::RenderPipelineDescriptor pipelineDesc;
	std::string shaderName = webGPUShader->GetName();
	pipelineDesc.label = shaderName.c_str();

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

	wgpu::VertexBufferLayout vertexBufferLayout;
	vertexBufferLayout.arrayStride = layout.GetStride();
	vertexBufferLayout.attributeCount = (uint32_t)vertexAttributes.size();
	vertexBufferLayout.attributes = vertexAttributes.data();
	vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

	// Vertex state
	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = "vs_main";
	pipelineDesc.vertex.bufferCount = 1;
	pipelineDesc.vertex.buffers = &vertexBufferLayout;

	wgpu::FragmentState fragmentState;
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";

	// Colour target state
	std::vector<wgpu::ColorTargetState> colourTargets;
	std::vector<wgpu::BlendState> blends;

	uint32_t targetCount = m_Specification.targetFormats.empty() ? 1 : (uint32_t)m_Specification.targetFormats.size();
	colourTargets.resize(targetCount);
	blends.resize(targetCount);

	if (m_Specification.targetFormats.empty())
	{
		colourTargets[0].format = m_WebGPUContext->GetSwapchainFormat();
		colourTargets[0].writeMask = wgpu::ColorWriteMask::All;

		if (m_Specification.transparencyEnabled)
		{
			blends[0].color.operation = wgpu::BlendOperation::Add;
			blends[0].color.srcFactor = wgpu::BlendFactor::SrcAlpha;
			blends[0].color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
			blends[0].alpha.operation = wgpu::BlendOperation::Add;
			blends[0].alpha.srcFactor = wgpu::BlendFactor::One;
			blends[0].alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
			colourTargets[0].blend = &blends[0];
		}
	}
	else
	{
		for (uint32_t i = 0; i < targetCount; i++)
		{
			colourTargets[i].format = FrameBufferFormatToWebGPU(m_Specification.targetFormats[i]);
			colourTargets[i].writeMask = wgpu::ColorWriteMask::All;

			if (m_Specification.transparencyEnabled && m_Specification.targetFormats[i] != FrameBufferTextureFormat::RED_INTEGER)
			{
				blends[i].color.operation = wgpu::BlendOperation::Add;
				blends[i].color.srcFactor = wgpu::BlendFactor::SrcAlpha;
				blends[i].color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
				blends[i].alpha.operation = wgpu::BlendOperation::Add;
				blends[i].alpha.srcFactor = wgpu::BlendFactor::One;
				blends[i].alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
				colourTargets[i].blend = &blends[i];
			}
		}
	}

	fragmentState.targetCount = targetCount;
	fragmentState.targets = colourTargets.data();
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
	else
	{
		ENGINE_TRACE("WebGPUPipeline: Creating pipeline WITHOUT depth stencil for shader: {0}", webGPUShader->GetName());
		pipelineDesc.depthStencil = nullptr;
	}
	pipelineDesc.multisample.count = 1;
	pipelineDesc.multisample.mask = 0xFFFFFFFF;
	pipelineDesc.multisample.alphaToCoverageEnabled = false;
	pipelineDesc.layout = nullptr;

	m_Pipeline = device.createRenderPipeline(pipelineDesc);
	if (!m_Pipeline)
	{
		ENGINE_ERROR("WebGPUPipeline: Failed to create render pipeline for shader: {0}", webGPUShader->GetName());
	}
}

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
	if (!m_Pipeline)
		return;
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