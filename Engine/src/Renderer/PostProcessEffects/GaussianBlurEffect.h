#pragma once
#include "Renderer/PostProcess.h"

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
};