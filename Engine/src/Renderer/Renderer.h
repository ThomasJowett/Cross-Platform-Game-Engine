#pragma once

#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"

class Renderer
{
public:
	static bool Init();
	static void BeginScene(Camera& camera);
	static void EndScene();

	static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform = Matrix4x4());

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
private:
	struct SceneData
	{
		Matrix4x4 ViewProjectionMatrix;
	};

	static SceneData* m_SceneData;
};