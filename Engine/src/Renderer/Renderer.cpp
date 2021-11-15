#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

#include "FrameBuffer.h"

struct SceneData
{
	Matrix4x4 viewProjectionMatrix;
};

/* ------------------------------------------------------------------------------------------------------------------ */

SceneData s_Data;

bool Renderer::Init()
{
	if (RenderCommand::Init())
		return Renderer2D::Init();
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Shutdown()
{
	Renderer2D::Shutdown();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::BeginScene(const Matrix4x4& transform, const Matrix4x4& projection)
{
	s_Data.viewProjectionMatrix = projection * Matrix4x4::Inverse(transform);
	Renderer2D::BeginScene(transform, projection);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::EndScene()
{
	Renderer2D::EndScene();
	//TODO: frustum culling
	//TODO: sort the opaque objects front to back
	//TODO: sort the transparent objects back to front
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform)
{
	//TODO: submit the vertex array to a render queue
	shader->Bind();
	shader->SetMat4("u_ViewProjection", s_Data.viewProjectionMatrix, true);
	shader->SetMat4("u_ModelMatrix", transform, true);

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Submit(const Material& material, const Mesh& mesh, const Matrix4x4& transform)
{
	Ref<Shader> shader = material.GetShader();

	if (!shader)
		return;

	shader->Bind();
	shader->SetMat4("u_ViewProjection", s_Data.viewProjectionMatrix, true);
	shader->SetMat4("u_ModelMatrix", transform, true);

	shader->SetFloat4("u_colour", material.GetTint());

	//TODO: each texture should have it's own tiling factor
	shader->SetFloat("u_tilingFactor", 1.0f);

	material.BindTextures();

	Ref<VertexArray> vertexArray = mesh.GetVertexArray();

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}
