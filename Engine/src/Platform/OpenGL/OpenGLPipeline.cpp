#include "OpenGLPipeline.h"
#include "OpenGLRendererAPI.h"
#include "Renderer/RenderCommand.h"

#include <glad/glad.h>

OpenGLPipeline::OpenGLPipeline(const Spec& spec)
{
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
void OpenGLPipeline::SetTexture(Ref<Texture> texture, uint32_t binding, Ref<Sampler> sampler, uint32_t set)
{
	if (texture)
	{
		texture->Bind(binding);
		// Without a sampler, use the texture's own filter/wrap rather than one left bound by an earlier draw
		if (sampler)
			sampler->Bind(binding);
		else
			glBindSampler(binding, 0);
	}
}

// TODO(texture-array-cleanup): GLSL's sampler2D[8] already dynamically indexes fine - this would
// only need to change if SetTextureArray moves to a true array texture (see the WebGPU TODO).
void OpenGLPipeline::SetTextureArray(const std::vector<Ref<Texture>>& textures, uint32_t firstBinding, Ref<Sampler> sampler, uint32_t set)
{
	for (size_t i = 0; i < textures.size(); i++)
	{
		if (!textures[i])
			continue;
		textures[i]->Bind(firstBinding + (uint32_t)i);
		if (sampler)
			sampler->Bind(firstBinding + (uint32_t)i);
	}
}

void OpenGLPipeline::Bind()
{
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

	if (m_Specification.backFaceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (m_Specification.transparencyEnabled)
	{
		glEnable(GL_BLEND);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	else
	{
		glDisable(GL_BLEND);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	static_cast<OpenGLRendererAPI&>(RenderCommand::Get()).SetCurrentPipeline(shared_from_this());
}

void OpenGLPipeline::ConfigureVertexBuffer(const VertexBuffer* vertexBuffer)
{
	m_VertexArray->Bind();

	if (vertexBuffer == m_LastConfiguredVertexBuffer)
		return;

	m_VertexArray->AddVertexBuffer(vertexBuffer, m_Specification.layout);
	m_LastConfiguredVertexBuffer = vertexBuffer;
}