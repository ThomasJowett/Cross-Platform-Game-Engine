#include "Renderer.h"
#include "Renderer2D.h"
#include "RenderCommand.h"
#include "RenderPipeline.h"
#include "Pipeline.h"

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

	std::unordered_map<std::string, Ref<Pipeline>> pipelineCache;

	Renderer::Stats stats;
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

	// One shared modelUniformBuffer, rewritten per draw via queue.writeBuffer(), does not work on
	// WebGPU: writeBuffer calls apply immediately relative to the queue, but every draw in a render
	// pass is recorded into one command encoder and only submitted once, as a batch, at the end - by
	// the time any of them actually execute on the GPU every earlier writeBuffer call has already
	// landed, so all draws in the batch end up reading whichever draw's data was written last. Each
	// draw needs its own buffer object instead; this pool hands out (creating on demand) a fresh one
	// per draw and is reset to reuse from the start once per frame.
	std::vector<Ref<UniformBuffer>> modelUniformBufferPool;
	size_t modelUniformBufferPoolIndex = 0;
};

/* ------------------------------------------------------------------------------------------------------------------ */

RendererData s_RendererData;
SceneData s_SceneData;
ShaderLibrary s_ShaderLibrary;
Scope<RenderPipeline> s_RenderPipeline;

std::vector<Command> s_OpaqueRenderQueue;
std::vector<Command> s_TransparentRenderQueue;

/* ------------------------------------------------------------------------------------------------------------------ */

static Ref<UniformBuffer> NextModelUniformBuffer()
{
	if (s_SceneData.modelUniformBufferPoolIndex >= s_SceneData.modelUniformBufferPool.size())
		s_SceneData.modelUniformBufferPool.push_back(UniformBuffer::Create(sizeof(SceneData::ModelBuffer), 1));

	return s_SceneData.modelUniformBufferPool[s_SceneData.modelUniformBufferPoolIndex++];
}

/* ------------------------------------------------------------------------------------------------------------------ */

static Ref<Pipeline> GetPipeline(const Ref<Shader>& shader, const BufferLayout& layout, bool transparent, bool twoSided)
{
	std::string key = shader->GetName() + (transparent ? "_trans" : "_opaque") + (twoSided ? "_twosided" : "_cull");
	if (s_RendererData.pipelineCache.find(key) != s_RendererData.pipelineCache.end())
		return s_RendererData.pipelineCache[key];

	Pipeline::Spec spec;
	spec.shader = shader;
	spec.layout = layout;
	spec.transparencyEnabled = transparent;
	// WebGPU bakes cull mode into the pipeline at creation time rather than allowing it to be
	// toggled per draw call (unlike OpenGL, where DrawIndexed's backFaceCull parameter can flip
	// glEnable/glDisable(GL_CULL_FACE) on the fly) - so a two-sided material needs its own pipeline
	// variant here, since setting Material::SetTwoSided() alone has no effect on WebGPU otherwise.
	spec.backFaceCulling = !twoSided;
	spec.targetFormats = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER };
	spec.hasDepth = true;

	Ref<Pipeline> pipeline = Pipeline::Create(spec);
	s_RendererData.pipelineCache[key] = pipeline;
	return pipeline;
}

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
			continue;

		Ref<Pipeline> pipeline = GetPipeline(shader, command.mesh->GetVertexLayout(), command.material->IsTransparent(), command.material->IsTwoSided());
		// pipeline is never null here - Pipeline::Create always returns an object even when the
		// underlying graphics-API pipeline failed to build (e.g. its shader couldn't be loaded, such
		// as a missing .wgsl file). Binding buffers and drawing against no valid bound pipeline is
		// what was crashing - skip the whole command instead.
		if (!pipeline || !pipeline->IsValid())
			continue;

		Ref<UniformBuffer> modelUniformBuffer = NextModelUniformBuffer();

		pipeline->Bind();
		pipeline->SetUniformBuffer(s_SceneData.constantUniformBuffer, 0);
		pipeline->SetUniformBuffer(modelUniformBuffer, 1);

		s_SceneData.modelBuffer.modelMatrix = command.transform.GetTranspose();
		s_SceneData.modelBuffer.colour = command.material->GetTint();
		s_SceneData.modelBuffer.textureOffset = command.material->GetTextureOffset();
		s_SceneData.modelBuffer.tilingFactor = command.material->GetTilingFactor();
		s_SceneData.modelBuffer.entityId = command.entityId;
		modelUniformBuffer->SetData(&s_SceneData.modelBuffer, sizeof(SceneData::ModelBuffer));

		s_RendererData.whiteTexture->Bind(0);
		s_RendererData.normalTexture->Bind(1);
		s_RendererData.mixMapTexture->Bind(2);

		if (s_RendererData.drawMode == DrawMode::FILL)
			command.material->BindTextures();

		// Texture::Bind() above only does anything on OpenGL (WebGPU has no global texture-unit
		// binding - it's always relative to a specific pipeline's bind group), so WebGPU also needs
		// this explicit, pipeline-scoped equivalent.
		Ref<Texture> albedo = command.material->GetTexture(0);
		pipeline->SetTexture(albedo ? albedo : s_RendererData.whiteTexture, 2);

		command.mesh->GetVertexBuffer()->Bind();
		command.mesh->GetIndexBuffer()->Bind();
		RenderCommand::DrawIndexed(command.indexCount, command.startIndex, command.vertexOffset, !command.material->IsTwoSided(), s_RendererData.drawMode);
		command.mesh->GetIndexBuffer()->UnBind();
		command.mesh->GetVertexBuffer()->UnBind();

		s_RendererData.stats.drawCalls++;
		s_RendererData.stats.meshCount++;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Renderer::Init()
{
	s_SceneData.constantUniformBuffer = UniformBuffer::Create(sizeof(SceneData::ConstantBuffer), 0);

	uint32_t whiteTextureData = Colour(Colours::WHITE).HexValue();
	s_RendererData.whiteTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, 1u, &whiteTextureData);

	uint32_t normalTextureData = Colour(0.5f, 0.5f, 1.0f, 1.0f).HexValue();
	s_RendererData.normalTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, 1u, &normalTextureData);

	uint32_t mixMapTextureData = Colour(0.5f, 0.0f, 0.5f, 1.0f).HexValue();
	s_RendererData.mixMapTexture = Texture2D::Create(1, 1, Texture2D::Format::RGBA, 1u, &mixMapTextureData);

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
	ENGINE_TRACE("Renderer: BeginScene");

	// WebGPU's clip-space Y axis points the opposite way to OpenGL's (matching Vulkan/D3D/Metal),
	// so without this every WebGPU-rendered frame comes out vertically flipped relative to OpenGL -
	// the whole scene upside down, physics appearing to fall the wrong way, etc. This is the single
	// point all rendering (3D meshes, Renderer2D sprites, UI, editor gizmo overlays) gets its
	// view-projection from, so correcting it here fixes all of those consistently in one place
	// rather than needing a fix in every shader or render target.
	Matrix4x4 correctedProjection = projection;
	if (RendererAPI::GetAPI() == RendererAPI::API::WebGPU)
		correctedProjection = Matrix4x4::Scale(Vector3f(1.0f, -1.0f, 1.0f)) * correctedProjection;

	s_SceneData.constantBuffer.viewProjectionMatrix = (correctedProjection * Matrix4x4::Inverse(transform)).GetTranspose();
	s_SceneData.constantBuffer.eyePosition = transform.ExtractTranslation();

	s_SceneData.constantUniformBuffer->SetData(&s_SceneData.constantBuffer, sizeof(SceneData::ConstantBuffer));
	Renderer2D::BeginScene();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Renderer::EndScene()
{
	ENGINE_TRACE("Renderer: EndScene (opaque={0}, trans={1})", s_OpaqueRenderQueue.size(), s_TransparentRenderQueue.size());
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

	s_SceneData.modelUniformBufferPoolIndex = 0;
	RenderCommandForQueue(s_OpaqueRenderQueue);
	RenderCommandForQueue(s_TransparentRenderQueue);

	s_OpaqueRenderQueue.clear();
	s_TransparentRenderQueue.clear();
}

Ref<UniformBuffer> Renderer::GetConstantUniformBuffer()
{
	return s_SceneData.constantUniformBuffer;
}

const Renderer::Stats& Renderer::GetStats()
{
	return s_RendererData.stats;
}

void Renderer::ResetStats()
{
	s_RendererData.stats = Renderer::Stats();
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

	ENGINE_TRACE("Renderer: Submit mesh");
	Command command;
	command.entityId = entityId;
	command.indexCount = indexCount ? indexCount : mesh->GetIndexCount();
	command.startIndex = startIndex;
	command.vertexOffset = vertexOffset;
	command.material = material ? material.get() : Material::GetDefaultMaterial().get();
	command.mesh = mesh.get();
	command.transform = transform;

	if (command.material->IsTransparent())
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
