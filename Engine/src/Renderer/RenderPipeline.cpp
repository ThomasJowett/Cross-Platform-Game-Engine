#include "RenderPipeline.h"
#include "Logging/Instrumentor.h"
#include "Utilities/GeometryGenerator.h"
#include "RenderCommand.h"
#include "FrameBuffer.h"
#include "Renderer/Renderer.h"

#include "PostProcessEffects/GaussianBlurEffect.h"

RenderPipeline::RenderPipeline()
	:m_PostProcessStack()
{
	m_FullscreenQuad = GeometryGenerator::CreateFullScreenQuad();
	m_SceneFrameBuffer = FrameBuffer::Create({ m_WindowWidth, m_WindowHeight, { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth } });

	FrameBufferSpecification pingPongSpec = { m_WindowWidth, m_WindowHeight };
	pingPongSpec.attachments = { FrameBufferTextureFormat::RGBA8 };

	m_PingFrameBuffer = FrameBuffer::Create(pingPongSpec);
	m_PongFrameBuffer = FrameBuffer::Create(pingPongSpec);

	m_FinalPassShader = Renderer::GetShader("FinalPass", true);
	//m_FinalPassShader = Renderer::GetShader("EntityIdEffect", true);

	m_PostProcessStack.AddEffect(std::make_shared<GaussianBlurEffect>(1.0f));
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
		m_PingFrameBuffer->Resize(width, height);
		m_PongFrameBuffer->Resize(width, height);
	}

	m_SceneFrameBuffer->Bind();
	RenderCommand::Clear();
	m_SceneFrameBuffer->ClearAttachment(1, -1);
	scene->Render(view, projection);
	m_SceneFrameBuffer->UnBind();

	Ref<Texture> sceneTexture = m_SceneFrameBuffer->GetColourAttachment(0);
	Ref<Texture> depthTexture = m_SceneFrameBuffer->GetDepthAttachment();
	Ref<Texture> entityIdTexture = m_SceneFrameBuffer->GetColourAttachment(1);

	m_PostProcessStack.Execute(sceneTexture, depthTexture, entityIdTexture, m_PingFrameBuffer, m_PongFrameBuffer, m_FullscreenQuad);

	if (finalOutputTarget) {
		finalOutputTarget->Bind();
	}
	RenderCommand::Clear();

	m_FinalPassShader->Bind();

	Ref<Texture> outputTexture = m_PostProcessStack.empty() ? sceneTexture : m_PostProcessStack.GetFinalTexture();

	outputTexture->Bind(0);
	entityIdTexture->Bind(1);

	m_FullscreenQuad->GetVertexBuffer()->Bind();
	m_FullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(m_FullscreenQuad->GetIndexCount(), 0, 0, false);
	m_FullscreenQuad->GetIndexBuffer()->UnBind();
	m_FullscreenQuad->GetVertexBuffer()->UnBind();

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
