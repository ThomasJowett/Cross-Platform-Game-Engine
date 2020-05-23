#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

struct SceneData
{
	Matrix4x4 ViewProjectionMatrix;
};

SceneData s_Data;

bool Renderer::Init()
{
	if(RenderCommand::Init())
		return Renderer2D::Init();
	return false;
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(const Camera& camera)
{
	s_Data.ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform)
{
	shader->Bind();
	shader->SetMat4("u_ViewProjection", s_Data.ViewProjectionMatrix, true);
	shader->SetMat4("u_ModelMatrix", transform, true);

	CORE_ASSERT(vertexArray, "No data in vertex array");

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
