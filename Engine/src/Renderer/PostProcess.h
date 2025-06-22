#pragma once
#include "FrameBuffer.h"
#include <Asset/Shader.h>
#include <Asset/Texture.h>
#include "Mesh.h"
#include "UniformBuffer.h"

struct alignas(16) PostProcessData
{
	Vector2f screenSize = { 1920.0f, 1080.0f };
	float time = 0.0f;
	float deltaTime = 0.0f;

	float customParam1 = 0.0f;
	float customParam2 = 0.0f;
	float customParam3 = 0.0f;
	float customParam4 = 0.0f;
};

class PostProcessEffect {
public:
	PostProcessEffect() = default;
	virtual ~PostProcessEffect() = default;

	Ref<Shader> GetShader() const;

	virtual void Apply(Ref<Texture> colourTexture, Ref<Texture> depthTexture, Ref<Texture> entityIdTexture,
		const Ref<FrameBuffer> ping, const Ref<FrameBuffer> pong,
		Ref<Mesh> fullscreenQuad,
		PostProcessData& data, Ref<UniformBuffer> postProcessBuffer) = 0;

	virtual void SetData(PostProcessData& data) {}

protected:
	Ref<Shader> m_Shader;
};


class PostProcessStack {

public:
	PostProcessStack();

	void AddEffect(const Ref<PostProcessEffect> effect);
	void RemoveEffect(const Ref<PostProcessEffect> effect);
	void ClearEffects();

	void Execute(Ref<Texture> colourTexture, Ref<Texture> depthTexture, Ref<Texture> entityIdTexture, const Ref<FrameBuffer> ping, const Ref<FrameBuffer> pong, Ref<Mesh> fullscreenQuad);

	Ref<Texture> GetFinalTexture() const {
		return m_FinalOutput;
	}

	bool empty() const {
		return m_Effects.empty();
	}

private:
	std::vector<Ref<PostProcessEffect>> m_Effects;

	Ref<Texture> m_FinalOutput;

	Ref<UniformBuffer> m_PostProcessUniformBuffer;
	PostProcessData m_PostProcessData;
};
