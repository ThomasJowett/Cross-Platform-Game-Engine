#pragma once

#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

class Renderer
{
public:
	static bool Init();
	static void OnWindowResize(uint32_t width, uint32_t height);
	static void BeginScene(const Camera& camera);
	static void EndScene();

	static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform = Matrix4x4());
	static void Submit(const Material& material, const Mesh& vertexArray, const Matrix4x4& transform = Matrix4x4());

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};