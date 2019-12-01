#pragma once

#include "stdafx.h"
#include "Renderer/Buffer.h"

class VertexArray
{
public:
	virtual ~VertexArray() = default;

	virtual void Bind() const = 0;
	virtual void UnBind() const = 0;

	virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
	virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

	virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

	static VertexArray* Create();
};