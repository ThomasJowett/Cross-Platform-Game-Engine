#pragma once
#include "Renderer/PostProcess.h"
#include "Renderer/Pipeline.h"

class GaussianBlurEffect : public PostProcessEffect
{
public:
	GaussianBlurEffect(float strength);
	virtual ~GaussianBlurEffect() = default;

	virtual void Apply(Ref<Texture> colourTexture, Ref<Texture> depthTexture, Ref<Texture> entityIdTexture,
		const Ref<FrameBuffer> ping, const Ref<FrameBuffer> pong,
		Ref<Mesh> fullscreenQuad,
		PostProcessData& data, Ref<UniformBuffer> postProcessBuffer) override;

private:
	float m_Strength = 1.0f;

	// Both passes share the same shader/layout/target format, so one pipeline covers both. Created
	// lazily on first Apply() since the constructor has no mesh/layout to build it from up front.
	Ref<Pipeline> m_Pipeline;
};