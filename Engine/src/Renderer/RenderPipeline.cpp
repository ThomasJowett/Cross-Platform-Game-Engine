#include "RenderPipeline.h"
#include "Logging/Instrumentor.h"
#include "Utilities/GeometryGenerator.h"
#include "RenderCommand.h"
#include "FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Pipeline.h"

#include "PostProcessEffects/GaussianBlurEffect.h"
#include "Renderer2D.h"

// Off by default: entity-id (RED_INTEGER) can't be multisampled (WebGPU has no
// MultisampleResolve capability for integer formats, and some adapters can't even create a
// multisampled integer texture at all without a non-portable feature), and Renderer2D's
// quad/circle/line/hairline/text pipelines are shared, global instances also used to draw into
// other, non-multisampled targets (e.g. the editor's grid/gizmo overlay pass) - a pipeline built
// without an entity-id target is incompatible with those. Raising this requires giving those
// other passes their own dedicated pipelines first; the resolve mechanism itself (colour via
// native resolveTarget on WebGPU, glBlitFramebuffer on OpenGL) is otherwise fully implemented.
static constexpr uint32_t k_MSAASamples = 1;

RenderPipeline::RenderPipeline()
	:m_PostProcessStack()
{
	m_FullscreenQuad = GeometryGenerator::CreateFullScreenQuad();

	FrameBufferSpecification sceneSpec = { m_WindowWidth, m_WindowHeight, { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth } };
	sceneSpec.samples = k_MSAASamples;
	m_SceneFrameBuffer = FrameBuffer::Create(sceneSpec);

	// With MSAA off, nothing ever writes into a second framebuffer - alias it directly to the
	// (non-multisampled) scene framebuffer instead of creating one that would just sit empty.
	if (k_MSAASamples > 1)
	{
		FrameBufferSpecification resolvedSpec = { m_WindowWidth, m_WindowHeight, { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth } };
		m_ResolvedSceneFrameBuffer = FrameBuffer::Create(resolvedSpec);
	}
	else
	{
		m_ResolvedSceneFrameBuffer = m_SceneFrameBuffer;
	}

	Renderer::SetTargetSamples(k_MSAASamples);
	Renderer2D::SetTargetSamples(k_MSAASamples);

	FrameBufferSpecification pingPongSpec = { m_WindowWidth, m_WindowHeight };
	pingPongSpec.attachments = { FrameBufferTextureFormat::RGBA8 };

	m_PingFrameBuffer = FrameBuffer::Create(pingPongSpec);
	m_PongFrameBuffer = FrameBuffer::Create(pingPongSpec);

	m_FinalPassShader = Renderer::GetShader("FinalPass", true);

	Pipeline::Spec finalPassSpec;
	finalPassSpec.shader = m_FinalPassShader;
	finalPassSpec.layout = m_FullscreenQuad->GetVertexLayout();
	finalPassSpec.backFaceCulling = false;
	finalPassSpec.depthTest = false;
	finalPassSpec.transparencyEnabled = true;
	finalPassSpec.targetFormats = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER };
	finalPassSpec.hasDepth = true;
	finalPassSpec.depthTest = false;
	m_FinalPassPipeline = Pipeline::Create(finalPassSpec);
}

RenderPipeline::~RenderPipeline()
{
}

void RenderPipeline::Render(Scene* scene, const Matrix4x4& view, const Matrix4x4& projection, Ref<FrameBuffer> finalOutputTarget)
{
	PROFILE_FUNCTION();

	uint32_t width, height;

	if (finalOutputTarget) {
		width = finalOutputTarget->GetSpecification().width;
		height = finalOutputTarget->GetSpecification().height;
	}
	else
	{
		width = m_WindowWidth;
		height = m_WindowHeight;
	}
	
	if (width != m_SceneFrameBuffer->GetSpecification().width ||
		height != m_SceneFrameBuffer->GetSpecification().height)
	{
		m_SceneFrameBuffer->Resize(width, height);
		if (m_ResolvedSceneFrameBuffer != m_SceneFrameBuffer)
			m_ResolvedSceneFrameBuffer->Resize(width, height);
		m_PingFrameBuffer->Resize(width, height);
		m_PongFrameBuffer->Resize(width, height);
	}

	m_SceneFrameBuffer->SetResolveTarget(m_ResolvedSceneFrameBuffer);

	m_SceneFrameBuffer->Bind();
	RenderCommand::StartRenderPass();
	RenderCommand::Clear();
	m_SceneFrameBuffer->ClearAttachment(1, -1);
	scene->Render(view, projection);
	RenderCommand::EndRenderPass();
	m_SceneFrameBuffer->UnBind();
	m_SceneFrameBuffer->ResolveTo(m_ResolvedSceneFrameBuffer);

	Ref<Texture> sceneTexture = m_ResolvedSceneFrameBuffer->GetColourAttachment(0);
	Ref<Texture> depthTexture = m_ResolvedSceneFrameBuffer->GetDepthAttachment();
	Ref<Texture> entityIdTexture = m_ResolvedSceneFrameBuffer->GetColourAttachment(1);

	m_PostProcessStack.Execute(sceneTexture, depthTexture, entityIdTexture, m_PingFrameBuffer, m_PongFrameBuffer, m_FullscreenQuad);

	if (finalOutputTarget) {
		finalOutputTarget->Bind();
	}
	RenderCommand::StartRenderPass();
	RenderCommand::Clear();

	m_FinalPassPipeline->Bind();
	m_FinalPassShader->Bind();

	Ref<Texture> outputTexture = m_PostProcessStack.empty() ? sceneTexture : m_PostProcessStack.GetFinalTexture();

	m_FinalPassPipeline->SetTexture(outputTexture, 0);
	m_FinalPassPipeline->SetTexture(entityIdTexture, 2);

	m_FullscreenQuad->GetVertexBuffer()->Bind();
	m_FullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(m_FullscreenQuad->GetIndexCount());
	m_FullscreenQuad->GetIndexBuffer()->UnBind();
	m_FullscreenQuad->GetVertexBuffer()->UnBind();

	scene->RenderUI(width, height);
	RenderCommand::EndRenderPass();

	if (finalOutputTarget) {
		m_SceneFrameBuffer->BlitDepthTo(finalOutputTarget);
		finalOutputTarget->UnBind();
	}
}

void RenderPipeline::Resize(uint32_t width, uint32_t height)
{
	PROFILE_FUNCTION();
	m_WindowWidth = width;
	m_WindowHeight = height;
}

void RenderPipeline::AddPostProcessEffect(const Ref<PostProcessEffect>& effect)
{
	m_PostProcessStack.AddEffect(effect);
}

void RenderPipeline::RemovePostProcessEffect(const Ref<PostProcessEffect>& effect)
{
	m_PostProcessStack.RemoveEffect(effect);
}

void RenderPipeline::ClearPostProcessEffects()
{
	m_PostProcessStack.ClearEffects();
}
