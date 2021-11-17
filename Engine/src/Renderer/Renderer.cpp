#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

#include "FrameBuffer.h"
#include "UniformBuffer.h"

struct SceneData
{
	__declspec(align(16)) struct ConstantBuffer
	{
		Matrix4x4 viewProjectionMatrix;
	};
	__declspec(align(16)) struct ModelBuffer
	{
		Matrix4x4 modelMatrix;
		Colour colour;
		float tilingFactor = 1.0f;
		int entityId = -1;
	};

	ConstantBuffer constantBuffer;
	ModelBuffer modelBuffer;
	
	Ref<UniformBuffer> constantUniformBuffer;
	Ref<UniformBuffer> modelUniformBuffer;
};

/* ------------------------------------------------------------------------------------------------------------------ */

SceneData s_Data;

bool Renderer::Init()
{
	s_Data.constantUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ConstantBuffer), 0);
	s_Data.modelUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ModelBuffer), 1);
	
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
	s_Data.constantBuffer.viewProjectionMatrix = (projection * Matrix4x4::Inverse(transform)).GetTranspose();

	s_Data.constantUniformBuffer->SetData(&s_Data.constantBuffer, sizeof(SceneData::ConstantBuffer));
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

void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4x4& transform, Colour colour, float tilingFactor, int entityId)
{
	//TODO: submit the vertex array to a render queue
	shader->Bind();

	s_Data.modelBuffer.modelMatrix = transform.GetTranspose();
	s_Data.modelBuffer.colour = colour;
	//TODO: each texture should have it's own tiling factor
	s_Data.modelBuffer.tilingFactor = tilingFactor;
	s_Data.modelBuffer.entityId = entityId;
	s_Data.modelUniformBuffer->SetData(&s_Data.modelBuffer, sizeof(SceneData::ModelBuffer));

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Submit(const Material& material, const Mesh& mesh, const Matrix4x4& transform, int entityId)
{
	Ref<Shader> shader = material.GetShader();

	if (!shader)
		return;

	shader->Bind();
	s_Data.modelBuffer.modelMatrix = transform.GetTranspose();
	s_Data.modelBuffer.colour = material.GetTint();
	//TODO: each texture should have it's own tiling factor
	s_Data.modelBuffer.tilingFactor = 1.0f;
	s_Data.modelBuffer.entityId = entityId;
	s_Data.modelUniformBuffer->SetData(&s_Data.modelBuffer, sizeof(SceneData::ModelBuffer));

	material.BindTextures();

	Ref<VertexArray> vertexArray = mesh.GetVertexArray();

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}
