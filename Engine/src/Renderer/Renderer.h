#pragma once

#include "RendererAPI.h"
#include "Camera.h"
#include "Asset/Shader.h"
#include "Asset/Material.h"
#include "Mesh.h"
#include "PostProcess.h"

#include "Scene/Components/StaticMeshComponent.h"

class Scene;
class FrameBuffer;
class Renderer
{
public:
	static bool Init();
	static void Shutdown();
	static void OnWindowResize(uint32_t width, uint32_t height);
	static void BeginScene(const Matrix4x4& transform, const Matrix4x4& projection);
	static void EndScene();

	static Ref<UniformBuffer> GetConstantUniformBuffer();

	// Sample count new mesh pipelines are created with - callers must set this before submitting
	// any draws that need it (it does not retroactively rebuild already-cached pipelines).
	static void SetTargetSamples(uint32_t samples);

	static void Submit(const Ref<Mesh> mesh, const Ref<Material> material, const Matrix4x4& transform = Matrix4x4(), int entityId = -1, uint32_t indexCount = 0, uint32_t startIndex = 0, uint32_t vertexOffset = 0);
	static void Submit(const Ref<Mesh> mesh, const Matrix4x4& transform = Matrix4x4(), int entityId = -1);
	static void Submit(const Ref<Mesh> mesh, const std::vector<Ref<Material>>& materials, const Matrix4x4& transform = Matrix4x4(), int entityId = -1);

	static void RenderScene(Scene* scene);
	static void RenderScene(Scene* scene, const Matrix4x4& view, const Matrix4x4& projection, Ref<FrameBuffer> finalOutputTarget = nullptr);

	static Ref<Shader> GetShader(const std::string& name, bool postProcess = false);

	static void AddPostProcessEffect(const Ref<PostProcessEffect>& effect);
	static void RemovePostProcessEffect(const Ref<PostProcessEffect>& effect);
	static void ClearPostProcessEffects();

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	struct Stats
	{
		uint32_t drawCalls = 0;
		uint32_t meshCount = 0;
	};

	static const Stats& GetStats();
	static void ResetStats();
};