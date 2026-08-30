#pragma once

class VertexBuffer;
class BufferLayout;

class OpenGLVertexArray
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	virtual void Bind() const;
	virtual void UnBind() const;

	// layout is passed explicitly (rather than read from vertexBuffer->GetLayout()) so the
	// caller - OpenGLPipeline - can drive this from its own Spec::layout, the single source
	// of truth already used to build the WebGPU vertex layout, instead of whatever this
	// specific buffer happened to be told last.
	virtual void AddVertexBuffer(const VertexBuffer* vertexBuffer, const BufferLayout& layout);
	//virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

	//virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const;
	//virtual const Ref<IndexBuffer>& GetIndexBuffer() const;

private:
	//std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	//Ref<IndexBuffer> m_IndexBuffer;

	uint32_t m_RendererID;
	uint32_t m_VertexBufferIndex = 0;
};