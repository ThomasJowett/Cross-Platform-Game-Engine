#include "stdafx.h"
#include "Mesh.h"
#include "Scene/AssetManager.h"

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, Ref<Material> material, const BufferLayout& layout)
	:m_Vertices(vertices), m_Indices(indices), m_VertexLayout(layout)
{
	const float* v = (float*)m_Vertices.data();
	// Generate the bounding box
	auto stride = layout.GetStride() / sizeof(float);
	m_Bounds.EnclosePoints(v, (uint32_t)m_Vertices.size() / stride, stride);

	Submesh submesh;
	submesh.firstIndex = 0;
	submesh.vertexOffset = 0;
	submesh.indexCount = (uint32_t)indices.size();
	submesh.boundingBox = m_Bounds;
	submesh.materialIndex = 0;
	submesh.localTransform = Matrix4x4();
	submesh.transform = Matrix4x4();
	m_Submeshes.push_back(submesh);

	m_Materials.push_back(material);

	m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(float)));
	m_VertexBuffer->SetLayout(layout);

	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
}

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const std::vector<Submesh>& submeshes, const std::vector<Ref<Material>>& materials, const BufferLayout& layout)
	:m_Vertices(vertices), m_Indices(indices), m_Submeshes(submeshes), m_Materials(materials), m_VertexLayout(layout)
{
	auto stride = layout.GetStride() / sizeof(float);
	m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(float));
	m_VertexBuffer->SetLayout(layout);
	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());

	m_Bounds.EnclosePoints(m_Vertices.data(), (uint32_t)m_Vertices.size() / stride, stride);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Submesh::SetBoundingBox(Vector3f min, Vector3f max)
{
	boundingBox = BoundingBox(min, max);

	ASSERT(boundingBox.IsValid(), "Bounding box must be valid!");
}