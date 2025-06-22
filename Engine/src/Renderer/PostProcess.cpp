
#include "PostProcess.h"
#include "Logging/Instrumentor.h"
#include "Renderer/RenderCommand.h"

PostProcessStack::PostProcessStack()
{
	PROFILE_FUNCTION();
	m_PostProcessUniformBuffer = UniformBuffer::Create(sizeof(PostProcessData), 3);
}

void PostProcessStack::AddEffect(const Ref<PostProcessEffect> effect)
{
	PROFILE_FUNCTION();
	if (effect)
	{
		m_Effects.push_back(effect);
	}
}

void PostProcessStack::RemoveEffect(const Ref<PostProcessEffect> effect)
{
	auto it = std::remove(m_Effects.begin(), m_Effects.end(), effect);
	if (it != m_Effects.end()) {
		m_Effects.erase(it, m_Effects.end());
	}
}

void PostProcessStack::ClearEffects()
{
	PROFILE_FUNCTION();
	m_Effects.clear();
}

void PostProcessStack::Execute(Ref<Texture> colourTexture, Ref<Texture> depthTexture, Ref<Texture> entityIdTexture, const Ref<FrameBuffer> ping, const Ref<FrameBuffer> pong, Ref<Mesh> fullscreenQuad)
{
	PROFILE_FUNCTION();

	bool pingIsSource = true;

	Ref<Texture> currentColour = colourTexture;

	m_PostProcessData.screenSize = Vector2f(static_cast<float>(colourTexture->GetWidth()), static_cast<float>(colourTexture->GetHeight()));

	m_PostProcessData.time += Application::GetDeltaTime();
	m_PostProcessData.deltaTime = Application::GetDeltaTime();

	for (size_t i = 0; i < m_Effects.size(); ++i)
	{
		auto& effect = m_Effects[i];
		Ref<FrameBuffer> outputTarget = pingIsSource ? pong : ping;

		outputTarget->Bind();

		effect->Apply(currentColour, depthTexture, entityIdTexture,
			pingIsSource ? ping : pong, pingIsSource ? pong : ping,
			fullscreenQuad, m_PostProcessData, m_PostProcessUniformBuffer);

		outputTarget->UnBind();

		currentColour = outputTarget->GetColourAttachment(0);
		pingIsSource = !pingIsSource;
	}

	m_FinalOutput = currentColour;
}

Ref<Shader> PostProcessEffect::GetShader() const
{
	return m_Shader;
}
