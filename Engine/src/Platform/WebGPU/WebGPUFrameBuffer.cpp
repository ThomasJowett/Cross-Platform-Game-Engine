#include "WebGPUFrameBuffer.h"
#include "WebGPUContext.h"
#include "WebGPURendererAPI.h"
#include "Logging/Instrumentor.h"

#include "Renderer/Pipeline.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Mesh.h"
#include "Utilities/GeometryGenerator.h"

#include <webgpu/webgpu.hpp>
#include "imgui/backends/imgui_impl_wgpu.h"

WebGPUFrameBuffer* WebGPUFrameBuffer::s_Current = nullptr;

WebGPUFrameBuffer::WebGPUFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification), m_DepthAttachment(0)
{
	PROFILE_FUNCTION();
	auto context = std::dynamic_pointer_cast<WebGPUContext>(Application::GetWindow()->GetContext());
	m_Device = context->GetWebGPUDevice();
	Generate();
}

WebGPUFrameBuffer::~WebGPUFrameBuffer()
{
	Destroy();
}

void WebGPUFrameBuffer::Bind()
{
	s_Current = this;
}

void WebGPUFrameBuffer::UnBind()
{
	if (s_Current == this)
		s_Current = nullptr;
}

WebGPUFrameBuffer* WebGPUFrameBuffer::GetCurrent() {
	return s_Current;
}

void WebGPUFrameBuffer::Generate()
{
	PROFILE_FUNCTION();

	Destroy();

	if (m_Specification.height == 0 || m_Specification.width == 0)
	{
		return;
	}

	bool multisample = m_Specification.samples > 1;

	// Attachments
	for (auto& spec : m_Specification.attachments.attachments)
	{
		if (FrameBuffer::IsDepthFormat(spec.textureFormat))
		{
			m_DepthAttachment = CreateRef<WebGPUTexture2D>(m_Specification.width, m_Specification.height,
				FrameBufferFormatToTextureFormat(spec.textureFormat),
				m_Specification.samples, nullptr);
			m_DepthView = m_DepthAttachment->GetTextureView();
		}
		else
		{
			auto colorTex = CreateRef<WebGPUTexture2D>(
				m_Specification.width, m_Specification.height,
				FrameBufferFormatToTextureFormat(spec.textureFormat),
				m_Specification.samples, nullptr);
			m_ColourViews.push_back(colorTex->GetTextureView());
			m_ColourAttachments.push_back(colorTex);
		}
	}
}

void WebGPUFrameBuffer::Destroy()
{
	PROFILE_FUNCTION();

	// Any of these colour attachments may have been displayed via ImGui::Image() (e.g. the
	// Editor viewport) - destroying their texture views here can hand a later-allocated view
	// the same address, which ImGui's WebGPU backend would otherwise mistake for the destroyed
	// one and reuse its now-invalid cached bind group, causing a validation error at that
	// point. Drop those cache entries first, before the views actually go away.
	//
	// Guarded: the macro only exists with the local, uncommitted imgui patch applied - keeps this
	// compiling against the real, unpatched submodule (CI, fresh clones).
#ifdef IMGUI_IMPL_WGPU_HAS_INVALIDATE_IMAGE_BIND_GROUPS
	if (!m_ColourAttachments.empty() || m_DepthAttachment)
		ImGui_ImplWGPU_InvalidateImageBindGroups();
#endif

	m_ColourAttachments.clear();
	m_DepthAttachment.reset();

	m_ColourViews.clear();
	m_DepthView = nullptr;
}

void WebGPUFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	PROFILE_FUNCTION();
	m_Specification.width = width;
	m_Specification.height = height;
	Destroy();
	Generate();
}

int WebGPUFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	PROFILE_FUNCTION();
	CORE_ASSERT(attachmentIndex < m_ColourAttachments.size(), "Trying to access attachment that does not exist!");
	int pixelData = m_ColourAttachments[attachmentIndex]->ReadPixel(x, y);
	return pixelData;
}

Ref<Texture> WebGPUFrameBuffer::GetColourAttachment(size_t index)
{
	CORE_ASSERT(index < m_ColourAttachments.size(), "Index out of range");
	return m_ColourAttachments[index];
}

Ref<Texture> WebGPUFrameBuffer::GetDepthAttachment()
{
	return m_DepthAttachment;
}

void WebGPUFrameBuffer::BlitDepthTo(Ref<FrameBuffer> target)
{
	PROFILE_FUNCTION();
	// WebGPU has no depth-to-depth copy/blit, so a fullscreen-quad pass reads the source depth
	// texture and writes it through via @builtin(frag_depth) instead.
	if (!m_DepthAttachment)
		return;

	auto targetWebGPU = std::dynamic_pointer_cast<WebGPUFrameBuffer>(target);
	if (!targetWebGPU || !targetWebGPU->GetDepthView())
		return;

	if (!m_DepthBlitPipeline)
	{
		Ref<Shader> shader = Renderer::GetShader("DepthBlit", true);
		m_DepthBlitFullscreenQuad = GeometryGenerator::CreateFullScreenQuad();

		Pipeline::Spec spec;
		spec.shader = shader;
		spec.layout = m_DepthBlitFullscreenQuad->GetVertexLayout();
		spec.backFaceCulling = false;
		spec.hasDepth = true;
		spec.depthOnly = true;
		spec.transparencyEnabled = false;
		spec.samples = 1;
		m_DepthBlitPipeline = Pipeline::Create(spec);
	}

	auto& rendererAPI = static_cast<WebGPURendererAPI&>(RenderCommand::Get());
	rendererAPI.StartDepthOnlyRenderPass(targetWebGPU->GetDepthView(),
		target->GetSpecification().width, target->GetSpecification().height);

	m_DepthBlitPipeline->Bind();
	m_DepthBlitPipeline->SetTexture(m_DepthAttachment, 0);

	m_DepthBlitFullscreenQuad->GetVertexBuffer()->Bind();
	m_DepthBlitFullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(m_DepthBlitFullscreenQuad->GetIndexCount(), 0, 0);
	m_DepthBlitFullscreenQuad->GetIndexBuffer()->UnBind();
	m_DepthBlitFullscreenQuad->GetVertexBuffer()->UnBind();

	RenderCommand::EndRenderPass();
}

void WebGPUFrameBuffer::BlitColourTo(Ref<FrameBuffer> target, uint32_t srcAttachmentIndex, uint32_t dstAttachmentIndex)
{
	PROFILE_FUNCTION();
	// Not natively supported in WebGPU, so we use a shader to copy the colour buffer
}

void WebGPUFrameBuffer::SetResolveTarget(Ref<FrameBuffer> target)
{
	m_ResolveTarget = target;
}

void WebGPUFrameBuffer::ResolveTo(Ref<FrameBuffer> target)
{
	PROFILE_FUNCTION();

	if (m_Specification.samples <= 1)
		return;

	// Find the entity-id (RED_INTEGER) attachment - integer formats have no MultisampleResolve
	// capability, so unlike colour (resolved natively via resolveTarget when the pass starts)
	// this one is still a genuine multisampled texture that needs a manual resolve.
	const auto& specAttachments = m_Specification.attachments.attachments;
	int entityIdIndex = -1;
	size_t colourIndex = 0;
	for (const auto& spec : specAttachments)
	{
		if (FrameBuffer::IsDepthFormat(spec.textureFormat))
			continue;
		if (spec.textureFormat == FrameBufferTextureFormat::RED_INTEGER)
		{
			entityIdIndex = (int)colourIndex;
			break;
		}
		colourIndex++;
	}

	if (entityIdIndex < 0)
		return;

	if (!m_EntityIdResolvePipeline)
	{
		Ref<Shader> shader = Renderer::GetShader("EntityIdResolve", true);
		m_ResolveFullscreenQuad = GeometryGenerator::CreateFullScreenQuad();

		Pipeline::Spec spec;
		spec.shader = shader;
		spec.layout = m_ResolveFullscreenQuad->GetVertexLayout();
		spec.backFaceCulling = false;
		spec.depthTest = false;
		spec.hasDepth = false;
		spec.transparencyEnabled = false;
		spec.targetFormats = { FrameBufferTextureFormat::RED_INTEGER };
		spec.samples = 1;
		m_EntityIdResolvePipeline = Pipeline::Create(spec);
	}

	Ref<Texture> multisampledEntityId = m_ColourAttachments[entityIdIndex];
	Ref<Texture> resolvedEntityId = target->GetColourAttachment(entityIdIndex);
	auto resolvedEntityIdWebGPU = std::dynamic_pointer_cast<WebGPUTexture2D>(resolvedEntityId);
	if (!resolvedEntityIdWebGPU)
		return;

	auto& rendererAPI = static_cast<WebGPURendererAPI&>(RenderCommand::Get());
	rendererAPI.StartSingleAttachmentRenderPass(resolvedEntityIdWebGPU->GetTextureView(),
		target->GetSpecification().width, target->GetSpecification().height);

	m_EntityIdResolvePipeline->Bind();
	m_EntityIdResolvePipeline->SetTexture(multisampledEntityId, 0);

	m_ResolveFullscreenQuad->GetVertexBuffer()->Bind();
	m_ResolveFullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(m_ResolveFullscreenQuad->GetIndexCount(), 0, 0);
	m_ResolveFullscreenQuad->GetIndexBuffer()->UnBind();
	m_ResolveFullscreenQuad->GetVertexBuffer()->UnBind();

	RenderCommand::EndRenderPass();
}

void WebGPUFrameBuffer::ClearAttachment(size_t index, int value)
{
	PROFILE_FUNCTION();
}