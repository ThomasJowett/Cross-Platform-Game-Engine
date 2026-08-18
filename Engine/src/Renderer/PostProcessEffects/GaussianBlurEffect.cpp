#include "GaussianBlurEffect.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

GaussianBlurEffect::GaussianBlurEffect(float strength)
{
	PROFILE_FUNCTION();
	m_Shader = Renderer::GetShader("BlurEffect", true);

	m_Strength = strength;
}

void GaussianBlurEffect::Apply(Ref<Texture> colourTexture, Ref<Texture> depthTexture, Ref<Texture> entityIdTexture,
	const Ref<FrameBuffer> ping, const Ref<FrameBuffer> pong,
	Ref<Mesh> fullscreenQuad,
	PostProcessData& data, Ref<UniformBuffer> postProcessBuffer)
{
	PROFILE_FUNCTION();

	if (!m_Pipeline)
	{
		Pipeline::Spec spec;
		spec.shader = m_Shader;
		spec.layout = fullscreenQuad->GetVertexLayout();
		spec.targetFormats = { FrameBufferTextureFormat::RGBA8 };
		spec.hasDepth = false;
		spec.depthTest = false;
		spec.backFaceCulling = false;
		m_Pipeline = Pipeline::Create(spec);
	}

	if (!m_Pipeline || !m_Pipeline->IsValid())
		return;

	data.customParam2 = m_Strength; // Set the strength of the blur effect

	// Horizontal pass: colourTexture -> ping
	data.customParam1 = true;
	postProcessBuffer->SetData(&data, sizeof(PostProcessData));

	ping->Bind();
	RenderCommand::StartRenderPass();
	m_Pipeline->Bind();
	m_Pipeline->SetTexture(colourTexture, 0);
	m_Pipeline->SetUniformBuffer(postProcessBuffer, 3);
	fullscreenQuad->GetVertexBuffer()->Bind();
	fullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(fullscreenQuad->GetIndexCount());
	fullscreenQuad->GetIndexBuffer()->UnBind();
	fullscreenQuad->GetVertexBuffer()->UnBind();
	RenderCommand::EndRenderPass();
	ping->UnBind();

	// Vertical pass: ping -> pong
	data.customParam1 = false;
	postProcessBuffer->SetData(&data, sizeof(PostProcessData));

	pong->Bind();
	RenderCommand::StartRenderPass();
	m_Pipeline->Bind();
	m_Pipeline->SetTexture(ping->GetColourAttachment(0), 0);
	m_Pipeline->SetUniformBuffer(postProcessBuffer, 3);
	fullscreenQuad->GetVertexBuffer()->Bind();
	fullscreenQuad->GetIndexBuffer()->Bind();
	RenderCommand::DrawIndexed(fullscreenQuad->GetIndexCount());
	fullscreenQuad->GetIndexBuffer()->UnBind();
	fullscreenQuad->GetVertexBuffer()->UnBind();
	RenderCommand::EndRenderPass();
	pong->UnBind();
}
