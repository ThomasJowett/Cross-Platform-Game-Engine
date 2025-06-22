#pragma once
#include "Scene/Scene.h"
#include "PostProcess.h"
#include "Mesh.h"

class RenderPipeline {
public:
	RenderPipeline();
	~RenderPipeline();

	void Render(Scene* scene, const Matrix4x4& view, const Matrix4x4& projection, Ref<FrameBuffer> finalOutput);

	void Resize(uint32_t width, uint32_t height);

	void AddPostProcessEffect(const Ref<PostProcessEffect>& effect);
	void RemovePostProcessEffect(const Ref<PostProcessEffect>& effect);
	void ClearPostProcessEffects();

private:
	PostProcessStack m_PostProcessStack;

	Ref<FrameBuffer> m_SceneFrameBuffer;
	Ref<FrameBuffer> m_PingFrameBuffer;
	Ref<FrameBuffer> m_PongFrameBuffer;

	Ref<Mesh> m_FullscreenQuad;
	Ref<Shader> m_FinalPassShader;

	uint32_t m_WindowWidth = 1920;
	uint32_t m_WindowHeight = 1080;
};
