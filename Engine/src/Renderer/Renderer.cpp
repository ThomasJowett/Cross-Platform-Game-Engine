#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"

#include "FrameBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"

#include "Core/core.h"

struct RendererData
{
	Ref<Texture> whiteTexture;
	Ref<Texture> normalTexture;
	Ref<Texture> mixMapTexture;
};

struct SceneData
{
	ALIGNED_TYPE(struct, 16)
	//typedef struct ALIGNED_(16) tagSTRUCTALIGNED16
	{
	    Matrix4x4 viewProjectionMatrix;
	}ConstantBuffer;

	typedef struct ALIGNED_(16) tagSTRUCTALIGNED16
	{
		Matrix4x4 modelMatrix;
		Colour colour;
		float tilingFactor = 1.0f;
		int entityId = -1;
	}ModelBuffer;

	ConstantBuffer constantBuffer;
	ModelBuffer modelBuffer;
	
	Ref<UniformBuffer> constantUniformBuffer;
	Ref<UniformBuffer> modelUniformBuffer;
};

/* ------------------------------------------------------------------------------------------------------------------ */

RendererData s_RendererData;
SceneData s_SceneData;
ShaderLibrary s_ShaderLibary;

bool Renderer::Init()
{
	s_SceneData.constantUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ConstantBuffer), 0);
	s_SceneData.modelUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ModelBuffer), 1);

	s_RendererData.whiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_RendererData.whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	s_RendererData.normalTexture = Texture2D::Create(1, 1);
	uint32_t normalTextureData = Colour(0.5f, 0.5f, 1.0f, 1.0f).HexValue();
	s_RendererData.normalTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	s_RendererData.mixMapTexture = Texture2D::Create(1, 1);
	uint32_t mixMapTextureData = Colour(0.5f, 0.0f, 0.5f, 1.0f).HexValue();
	s_RendererData.mixMapTexture->SetData(&whiteTextureData, sizeof(uint32_t));
	
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
	s_SceneData.constantBuffer.viewProjectionMatrix = (projection * Matrix4x4::Inverse(transform)).GetTranspose();

	s_SceneData.constantUniformBuffer->SetData(&s_SceneData.constantBuffer, sizeof(SceneData::ConstantBuffer));
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

	s_SceneData.modelBuffer.modelMatrix = transform.GetTranspose();
	s_SceneData.modelBuffer.colour = colour;
	//TODO: each texture should have it's own tiling factor
	s_SceneData.modelBuffer.tilingFactor = tilingFactor;
	s_SceneData.modelBuffer.entityId = entityId;
	s_SceneData.modelUniformBuffer->SetData(&s_SceneData.modelBuffer, sizeof(SceneData::ModelBuffer));

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Submit(const Ref<Material>& material, const Ref<Mesh>& mesh, const Matrix4x4& transform, int entityId)
{
	if (!material || !mesh)
		return;

	Ref<Shader> shader = s_ShaderLibary.Load(material->GetShader());

	if (!shader)
		return;

	shader->Bind();
	s_SceneData.modelBuffer.modelMatrix = transform.GetTranspose();
	s_SceneData.modelBuffer.colour = material->GetTint();
	//TODO: each texture should have it's own tiling factor
	s_SceneData.modelBuffer.tilingFactor = 1.0f;
	s_SceneData.modelBuffer.entityId = entityId;
	s_SceneData.modelUniformBuffer->SetData(&s_SceneData.modelBuffer, sizeof(SceneData::ModelBuffer));

	s_RendererData.whiteTexture->Bind(0);
	s_RendererData.normalTexture->Bind(1);
	s_RendererData.mixMapTexture->Bind(2);

	material->BindTextures();

	Ref<VertexArray> vertexArray = mesh->GetVertexArray();

	CORE_ASSERT(vertexArray, "No data in vertex array");

	RenderCommand::DrawIndexed(vertexArray);
}
