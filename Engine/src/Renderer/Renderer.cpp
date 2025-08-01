#include "stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"
#include "RenderPipeline.h"


#include "FrameBuffer.h"
#include "UniformBuffer.h"
#include "Asset/Texture.h"
#include "Scene/Entity.h"
#include "Scene/Components/CameraComponent.h"

#include "Core/core.h"

struct Command
{
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	uint32_t indexCount = 0;
	uint32_t startIndex = 0;
	uint32_t vertexOffset = 0;
	Matrix4x4 transform;
	int entityId = -1;
};

struct RendererData
{
	Ref<Texture> whiteTexture;
	Ref<Texture> normalTexture;
	Ref<Texture> mixMapTexture;

	DrawMode drawMode = DrawMode::FILL;
};

struct SceneData
{
	ALIGNED_TYPE(struct, 16)
	{
		Matrix4x4 viewProjectionMatrix;
		Vector3f eyePosition;
	}ConstantBuffer;

	typedef struct ALIGNED_(16) tagSTRUCTALIGNED16
	{
		Matrix4x4 modelMatrix;
		Colour colour;
		Vector2f textureOffset;
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
ShaderLibrary s_ShaderLibrary;
Scope<RenderPipeline> s_RenderPipeline;

std::vector<Command> s_OpaqueRenderQueue;
std::vector<Command> s_TransparentRenderQueue;

/* ------------------------------------------------------------------------------------------------------------------ */

void RenderCommandForQueue(const std::vector<Command>& renderQueue)
{
	for (const auto& command : renderQueue)
	{
		Ref<Shader> shader;
		if (s_RendererData.drawMode == DrawMode::WIREFRAME)
		{
			//TODO write wireframe geometry shader
			//shader = s_ShaderLibrary.Load("Wireframe");
		}
		else {
			shader = s_ShaderLibrary.Load(command.material->GetShader());
		}

		if (!shader)
			return;

		shader->Bind();
		s_SceneData.modelBuffer.modelMatrix = command.transform.GetTranspose();
		s_SceneData.modelBuffer.colour = command.material->GetTint();
		s_SceneData.modelBuffer.textureOffset = command.material->GetTextureOffset();
		s_SceneData.modelBuffer.tilingFactor = command.material->GetTilingFactor();
		s_SceneData.modelBuffer.entityId = command.entityId;
		s_SceneData.modelUniformBuffer->SetData(&s_SceneData.modelBuffer, sizeof(SceneData::ModelBuffer));

		s_RendererData.whiteTexture->Bind(0);
		s_RendererData.normalTexture->Bind(1);
		s_RendererData.mixMapTexture->Bind(2);

		if (s_RendererData.drawMode == DrawMode::FILL)
			command.material->BindTextures();

		command.mesh->GetVertexBuffer()->Bind();
		command.mesh->GetIndexBuffer()->Bind();
		RenderCommand::DrawIndexed(command.indexCount, command.startIndex, command.vertexOffset, !command.material->IsTwoSided(), s_RendererData.drawMode);
		command.mesh->GetIndexBuffer()->UnBind();
		command.mesh->GetVertexBuffer()->UnBind();
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Renderer::Init()
{
	s_SceneData.constantUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ConstantBuffer), 0);
	s_SceneData.modelUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ModelBuffer), 1);

	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_RendererData.whiteTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, false, &whiteTextureData);

	uint32_t normalTextureData = Colour(0.5f, 0.5f, 1.0f, 1.0f).HexValue();
	s_RendererData.normalTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, false, &normalTextureData);

	uint32_t mixMapTextureData = Colour(0.5f, 0.0f, 0.5f, 1.0f).HexValue();
	s_RendererData.mixMapTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, false, &mixMapTextureData);

	s_RenderPipeline = CreateScope<RenderPipeline>();

	if (RenderCommand::Init())
		return Renderer2D::Init();
	return false;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Shutdown()
{
	Renderer2D::Shutdown();
	s_RenderPipeline.reset();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RenderCommand::SetViewport(0, 0, width, height);
	if (s_RenderPipeline)
		s_RenderPipeline->Resize(width, height);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::BeginScene(const Matrix4x4& transform, const Matrix4x4& projection)
{
	s_SceneData.constantBuffer.viewProjectionMatrix = (projection * Matrix4x4::Inverse(transform)).GetTranspose();
	s_SceneData.constantBuffer.eyePosition = transform.ExtractTranslation();

	s_SceneData.constantUniformBuffer->SetData(&s_SceneData.constantBuffer, sizeof(SceneData::ConstantBuffer));
	Renderer2D::BeginScene();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::EndScene()
{
	Renderer2D::EndScene();
	//TODO: frustum culling

	// Sort the opqaue front to back to reduce over draw
	std::sort(s_OpaqueRenderQueue.begin(), s_OpaqueRenderQueue.end(), [](Command& a, Command& b)
		{
			return Vector3f::Distance(s_SceneData.constantBuffer.eyePosition, a.transform.ExtractTranslation()) <
				Vector3f::Distance(s_SceneData.constantBuffer.eyePosition, b.transform.ExtractTranslation());
		});

	// Sort the transparent back to front to render transparent correctly
	std::sort(s_TransparentRenderQueue.begin(), s_TransparentRenderQueue.end(), [](Command& a, Command& b)
		{
			return Vector3f::Distance(s_SceneData.constantBuffer.eyePosition, a.transform.ExtractTranslation()) <
				Vector3f::Distance(s_SceneData.constantBuffer.eyePosition, b.transform.ExtractTranslation());
		});

	RenderCommandForQueue(s_OpaqueRenderQueue);
	RenderCommandForQueue(s_TransparentRenderQueue);

	s_OpaqueRenderQueue.clear();
	s_TransparentRenderQueue.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::SetDrawMode(DrawMode drawMode)
{
	s_RendererData.drawMode = drawMode;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::Submit(const Ref<Mesh> mesh, const Ref<Material> material, const Matrix4x4& transform, int entityId, uint32_t indexCount, uint32_t startIndex, uint32_t vertexOffset)
{
	if (!mesh)
		return;
	Command command;
	command.entityId = entityId;
	command.indexCount = indexCount ? indexCount : mesh->GetIndexCount();
	command.startIndex = startIndex;
	command.vertexOffset = vertexOffset;
	command.material = material ? material.get() : Material::GetDefaultMaterial().get();
	command.mesh = mesh.get();
	command.transform = transform;

	if (material->IsTransparent())
	{
		s_TransparentRenderQueue.push_back(command);
	}
	else
	{
		s_OpaqueRenderQueue.push_back(command);
	}
}

void Renderer::Submit(const Ref<Mesh> mesh, const Matrix4x4& transform, int entityId)
{
	for (const auto& submesh : mesh->GetSubmeshes())
	{
		Submit(mesh, mesh->GetMaterials()[submesh.materialIndex], transform * submesh.transform, entityId, submesh.indexCount, submesh.firstIndex, submesh.vertexOffset);
	}
}

void Renderer::Submit(const Ref<Mesh> mesh, const std::vector<Ref<Material>>& materials, const Matrix4x4& transform, int entityId)
{
	for (const auto& submesh : mesh->GetSubmeshes())
	{
		Submit(mesh, materials[submesh.materialIndex], transform * submesh.transform, entityId, submesh.indexCount, submesh.firstIndex, submesh.vertexOffset);
	}
}

void Renderer::RenderScene(Scene* scene)
{
	PROFILE_FUNCTION();
	auto [view, projection] = scene->GetPrimaryCameraViewProjection();
	s_RenderPipeline->Render(scene, view, projection, nullptr);
}

void Renderer::RenderScene(Scene* scene, const Matrix4x4& view, const Matrix4x4& projection, Ref<FrameBuffer> finalOutputTarget)
{
	PROFILE_FUNCTION();
	s_RenderPipeline->Render(scene, view, projection, finalOutputTarget);
}

Ref<Shader> Renderer::GetShader(const std::string& name, bool postProcess)
{
	return s_ShaderLibrary.Load(name, postProcess);
}

void Renderer::AddPostProcessEffect(const Ref<PostProcessEffect>& effect)
{
	if (s_RenderPipeline)
		s_RenderPipeline->AddPostProcessEffect(effect);
}

void Renderer::RemovePostProcessEffect(const Ref<PostProcessEffect>& effect)
{
	if (s_RenderPipeline)
		s_RenderPipeline->RemovePostProcessEffect(effect);
}

void Renderer::ClearPostProcessEffects()
{
	if (s_RenderPipeline)
		s_RenderPipeline->ClearPostProcessEffects();
}
