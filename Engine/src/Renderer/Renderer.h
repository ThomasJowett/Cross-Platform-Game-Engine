#pragma once

#include "RendererAPI.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "Scene/Components/StaticMeshComponent.h"

class Renderer
{
public:
	static bool Init();
	static void Shutdown();
	static void OnWindowResize(uint32_t width, uint32_t height);
	static void BeginScene(const Matrix4x4& transform, const Matrix4x4& projection);
	static void EndScene();

	static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform = Matrix4x4(), int entity = -1);
	static void Submit(const Material& material, const Mesh& mesh, const Matrix4x4& transform = Matrix4x4(), int entityId = -1);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};