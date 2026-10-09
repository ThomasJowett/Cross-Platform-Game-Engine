#include "PostProcess.h"
#include "Logging/Instrumentor.h"

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

	uint32_t width = colourTexture->GetWidth();
	uint32_t height = colourTexture->GetHeight();

	if (!m_Scratch)
	{
		FrameBufferSpecification spec = { width, height };
		spec.attachments = { FrameBufferTextureFormat::RGBA8 };
		m_Scratch = FrameBuffer::Create(spec);
	}
	else if (m_Scratch->GetSpecification().width != width || m_Scratch->GetSpecification().height != height)
	{
		m_Scratch->Resize(width, height);
	}

	m_PostProcessData.screenSize = Vector2f(static_cast<float>(width), static_cast<float>(height));

	m_PostProcessData.time += Application::GetDeltaTime();
	m_PostProcessData.deltaTime = Application::GetDeltaTime();

	for (size_t i = 0; i < m_Effects.size(); ++i)
	{
		auto& effect = m_Effects[i];
		Ref<FrameBuffer> outputTarget = pingIsSource ? pong : ping;

		effect->Apply(currentColour, depthTexture, entityIdTexture,
			pingIsSource ? ping : pong, pingIsSource ? pong : ping, m_Scratch,
			fullscreenQuad, m_PostProcessData, m_PostProcessUniformBuffer);

		currentColour = outputTarget->GetColourAttachment(0);
		pingIsSource = !pingIsSource;
	}

	m_FinalOutput = currentColour;
}

Ref<Shader> PostProcessEffect::GetShader() const
{
	return m_Shader;
}
