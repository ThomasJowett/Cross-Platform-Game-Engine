#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

Scope<Renderer::SceneData> Renderer::m_SceneData = CreateScope<Renderer::SceneData>();

bool Renderer::Init()
{
	Renderer2D::Init();
	return RenderCommand::Init();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(Camera& camera)
{
	m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform)
{
	shader->Bind();
	shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix, true);
	shader->SetMat4("u_ModelMatrix", transform, true);

	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}
