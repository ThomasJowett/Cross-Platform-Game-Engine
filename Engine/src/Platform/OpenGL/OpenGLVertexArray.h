#pragma once

#include "Renderer/VertexArray.h"

class OpenGLVertexArray : public VertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)override;
	virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)override;


	virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override;
	virtual const Ref<IndexBuffer>& GetIndexBuffer() const override;

private:
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;

	uint32_t m_RendererID;
	
};