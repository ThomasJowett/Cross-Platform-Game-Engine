#pragma once

#include "RendererAPI.h"
#include "OrthographicCamera.h"
#include "Shader.h"

class Renderer
{
public:
	static void BeginScene(OrthographicCamera& camera);
	static void EndScene();

	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
private:
	struct SceneData
	{
		Matrix4x4 ViewProjectionMatrix;
	};

	static SceneData* m_SceneData;
};