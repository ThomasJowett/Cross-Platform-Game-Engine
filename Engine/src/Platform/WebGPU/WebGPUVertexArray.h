#pragma once

class VertexBuffer;

class WebGPUVertexArray
{
public:
	WebGPUVertexArray();
	~WebGPUVertexArray();

	virtual void Bind() const;
	virtual void UnBind() const;

	virtual void AddVertexBuffer(const VertexBuffer* vertexBuffer);
	//virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

	//virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const;
	//virtual const Ref<IndexBuffer>& GetIndexBuffer() const;

private:
	//std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	//Ref<IndexBuffer> m_IndexBuffer;

	uint32_t m_RendererID;
	uint32_t m_VertexBufferIndex = 0;
};