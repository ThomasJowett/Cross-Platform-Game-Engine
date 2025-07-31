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
	// Bind the shader and set the uniforms for the blur effect
	m_Shader->Bind();

	fullscreenQuad->GetVertexBuffer()->Bind();
	fullscreenQuad->GetIndexBuffer()->Bind();

	data.customParam1 = true;
	data.customParam2 = m_Strength; // Set the strength of the blur effect
	postProcessBuffer->SetData(&data, sizeof(PostProcessData));

	ping->Bind();
	colourTexture->Bind(0);

	RenderCommand::DrawIndexed(fullscreenQuad->GetIndexCount(), 0, 0, false);
	ping->UnBind();

	data.customParam1 = false;
	postProcessBuffer->SetData(&data, sizeof(PostProcessData));
	pong->Bind();
	ping->GetColourAttachment(0)->Bind(0);
	RenderCommand::DrawIndexed(fullscreenQuad->GetIndexCount(), 0, 0, false);
	pong->UnBind();

	fullscreenQuad->GetIndexBuffer()->UnBind();
	fullscreenQuad->GetVertexBuffer()->UnBind();
}
