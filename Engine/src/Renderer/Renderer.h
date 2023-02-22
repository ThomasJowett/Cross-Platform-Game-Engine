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

	static void SetDrawMode(DrawMode drawMode);

	static void Submit(const Ref<Mesh> mesh, const Ref<Material> material, const Matrix4x4& transform = Matrix4x4(), int entityId = -1, uint32_t indexCount = 0, uint32_t startIndex = 0, uint32_t vertexOffset = 0);
	static void Submit(const Ref<Mesh> mesh, const Matrix4x4& transform = Matrix4x4(), int entityId = -1);
	static void Submit(const Ref<Mesh> mesh, const std::vector<Ref<Material>>& materials, const Matrix4x4& transform = Matrix4x4(), int entityId = -1);

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};