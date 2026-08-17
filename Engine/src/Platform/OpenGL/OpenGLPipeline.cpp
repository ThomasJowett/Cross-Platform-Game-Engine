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
}
void OpenGLPipeline::SetTexture(Ref<Texture> texture, uint32_t binding, uint32_t set)
{
	if (texture)
		texture->Bind(binding);
}

void OpenGLPipeline::Bind()
{
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
	if (vertexBuffer == m_LastConfiguredVertexBuffer)
		return;

	m_VertexArray->AddVertexBuffer(vertexBuffer, m_Specification.layout);
	m_LastConfiguredVertexBuffer = vertexBuffer;
}