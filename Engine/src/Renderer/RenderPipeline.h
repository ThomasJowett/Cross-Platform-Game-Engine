#pragma once
#include "Renderer/Pipeline.h"
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
	// Always samples=1, same attachments as m_SceneFrameBuffer - what the scene actually resolves
	// into, and what everything downstream (post-process, final pass) reads from.
	Ref<FrameBuffer> m_ResolvedSceneFrameBuffer;
	Ref<FrameBuffer> m_PingFrameBuffer;
	Ref<FrameBuffer> m_PongFrameBuffer;

	Ref<Mesh> m_FullscreenQuad;
	Ref<Shader> m_FinalPassShader;
	Ref<Pipeline> m_FinalPassPipeline;

	// Used instead of m_FinalPassPipeline when rendering straight to the window's swapchain
	// (finalOutput == nullptr) - a presentation surface only has one colour attachment, in a
	// format the OS/windowing system chooses, so it can't use the same dual-attachment
	// (colour + entity-id) pipeline built for an offscreen target.
	Ref<Shader> m_FinalPassShaderSwapchain;
	Ref<Pipeline> m_FinalPassPipelineSwapchain;

	uint32_t m_WindowWidth = 1920;
	uint32_t m_WindowHeight = 1080;
};
