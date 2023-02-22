#include "stdafx.h"
#include "Mesh.h"
#include "Scene/AssetManager.h"

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Ref<Material> material)
	:m_Vertices(vertices), m_Indices(indices)
{
	const float* v = (float*)m_Vertices.data();
	// Generate the bounding box
	m_Bounds.EnclosePoints(v, (uint32_t)m_Vertices.size(), 11);

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

	m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
	m_VertexBuffer->SetLayout(s_StaticMeshLayout);

	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Submesh>& submeshes, const std::vector<Ref<Material>>& materials)
	:m_Vertices(vertices), m_Indices(indices), m_Submeshes(submeshes), m_Materials(materials)
{
	m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)indices.size());
	m_VertexBuffer->SetLayout(s_StaticMeshLayout);
	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)indices.size());

	m_Bounds.EnclosePoints((float*)m_Vertices.data(), (uint32_t)m_Vertices.size(), 11);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Submesh::SetBoundingBox(Vector3f min, Vector3f max)
{
	boundingBox = BoundingBox(min, max);

	ASSERT(boundingBox.IsValid(), "Bounding box must be valid!");
}