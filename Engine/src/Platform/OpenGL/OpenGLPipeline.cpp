#include "OpenGLPipeline.h"
#include "OpenGLRendererAPI.h"
#include "Renderer/RenderCommand.h"

#include <glad/glad.h>

OpenGLPipeline::OpenGLPipeline(const Spec& spec)
{
	m_TransparencyEnabled = spec.transparencyEnabled;
	m_BackfaceCull = spec.backFaceCulling;
	m_Specification = spec;
	Invalidate();
}

OpenGLPipeline::~OpenGLPipeline()
{
}

void OpenGLPipeline::Invalidate()
{
	m_VertexArray = CreateRef<OpenGLVertexArray>();
	m_LastConfiguredVertexBuffer = nullptr;
}

void OpenGLPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
{
	if (uniformBuffer)
		uniformBuffer->Bind();
}
void OpenGLPipeline::SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set)
{
	if (texture)
		texture->Bind(binding);
}

void OpenGLPipeline::SetTextureArray(const std::vector<Ref<Texture>>& textures, uint32_t firstBinding, Ref<Texture> samplerSource, uint32_t set)
{
	// samplerSource unused - GLSL's combined sampler2D needs no separate sampler binding.
	for (size_t i = 0; i < textures.size(); i++)
		if (textures[i])
			textures[i]->Bind(firstBinding + (uint32_t)i);
}

void OpenGLPipeline::Bind()
{
	// GL_CURRENT_PROGRAM is global, sticky GL state - without this, every draw silently kept
	// using whichever shader program was last bound (e.g. the post-process composite pass's
	// shader from the previous frame), since nothing else in this class ever calls glUseProgram.
	m_Specification.shader->Bind();

	if (m_Specification.hasDepth)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(m_Specification.depthTest ? GL_LESS : GL_ALWAYS);
		glDepthMask(m_Specification.depthTest ? GL_TRUE : GL_FALSE);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	static_cast<OpenGLRendererAPI&>(RenderCommand::Get()).SetCurrentPipeline(shared_from_this());
}

void OpenGLPipeline::ConfigureVertexBuffer(const VertexBuffer* vertexBuffer)
{
	// The VAO must always be (re)bound here, even when the attribute setup below is skipped -
	// something else (another pipeline's draw, ImGui's own rendering interleaved with ours)
	// may have bound a different VAO since this pipeline was last used, and skipping this
	// bind would silently draw against whatever VAO happens to be current instead of this
	// pipeline's own.
	m_VertexArray->Bind();

	if (vertexBuffer == m_LastConfiguredVertexBuffer)
		return;

	m_VertexArray->AddVertexBuffer(vertexBuffer, m_Specification.layout);
	m_LastConfiguredVertexBuffer = vertexBuffer;
}