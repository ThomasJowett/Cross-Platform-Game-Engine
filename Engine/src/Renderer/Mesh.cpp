#include "Mesh.h"
#include "Scene/AssetManager.h"
#include "RendererAPI.h"

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, Ref<Material> material, const BufferLayout& layout)
	:m_Vertices(vertices), m_Indices(indices), m_VertexLayout(layout)
{
	Submesh submesh;
	submesh.firstIndex = 0;
	submesh.vertexOffset = 0;
	submesh.indexCount = (uint32_t)indices.size();
	submesh.materialIndex = 0;
	submesh.localTransform = Matrix4x4();
	submesh.transform = Matrix4x4();
	m_Submeshes.push_back(submesh);

	CalculateBounds();
	m_Submeshes[0].boundingBox = m_Bounds;

	m_Materials.push_back(material);

	CreateBuffers();
}

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const std::vector<Submesh>& submeshes, const std::vector<Ref<Material>>& materials, const BufferLayout& layout)
	:m_Vertices(vertices), m_Indices(indices), m_Submeshes(submeshes), m_Materials(materials), m_VertexLayout(layout)
{
	CreateBuffers();
	CalculateBounds();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Mesh::CreateBuffers()
{
	// Headless runs keep only the CPU-side data
	if (RendererAPI::GetAPI() == RendererAPI::API::None)
		return;

	m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), (uint32_t)(m_Vertices.size() * sizeof(float)));
	m_VertexBuffer->SetLayout(m_VertexLayout);
	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Mesh::CalculateBounds()
{
	uint32_t stride = m_VertexLayout.GetStride() / sizeof(float);
	if (stride < 3)
		return;

	m_Bounds.Invalidate();

	// Submeshes are drawn with their own transform, so their vertices are enclosed after it's applied
	for (const Submesh& submesh : m_Submeshes)
	{
		uint32_t lastIndex = std::min(submesh.firstIndex + submesh.indexCount, (uint32_t)m_Indices.size());
		for (uint32_t i = submesh.firstIndex; i < lastIndex; ++i)
		{
			size_t offset = (size_t)(m_Indices[i] + submesh.vertexOffset) * stride;
			if (offset + 2 >= m_Vertices.size())
				continue;

			Vector3f position(m_Vertices[offset], m_Vertices[offset + 1], m_Vertices[offset + 2]);
			m_Bounds.Merge(submesh.transform * position);
		}
	}

	if (!m_Bounds.IsValid())
		m_Bounds.EnclosePoints(m_Vertices.data(), (uint32_t)m_Vertices.size() / stride, stride);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Submesh::SetBoundingBox(Vector3f min, Vector3f max)
{
	boundingBox = BoundingBox(min, max);

	ASSERT(boundingBox.IsValid(), "Bounding box must be valid!");
}