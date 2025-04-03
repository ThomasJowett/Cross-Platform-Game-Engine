#pragma once

#include "Buffer.h"
#include "Utilities/FileUtils.h"
#include "Core/Application.h"
#include "Core/Asset.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "Core/BoundingBox.h"
#include "Asset/Material.h"

struct Vertex
{
	Vector3f position;
	Vector3f normal;
	Vector3f tangent;
	Vector2f texcoord;

	bool operator==(const Vertex& other) const
	{
		return position == other.position
			&& normal == other.normal
			&& tangent == other.tangent
			&& texcoord == other.texcoord;
	}
};

struct SkinnedVertex
{
	Vector3f position;
	Vector3f normal;
	Vector3f tangent;
	Vector2f texcoord;
	Vector4f joint;
	Vector4f weight;

	bool operator==(const SkinnedVertex& other) const
	{
		return position == other.position
			&& normal == other.normal
			&& tangent == other.tangent
			&& texcoord == other.texcoord
			&& joint == other.joint
			&& weight == other.weight;
	}
};

static BufferLayout s_StaticMeshLayout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Normal"},
		{ShaderDataType::Float3, "a_Tangent"},
		{ShaderDataType::Float2, "a_TexCoord"}
};

struct Submesh
{
	uint32_t firstIndex;
	uint32_t vertexOffset;
	uint32_t indexCount;
	uint32_t vertexCount;

	uint32_t materialIndex;

	Matrix4x4 transform;
	Matrix4x4 localTransform;

	BoundingBox boundingBox;
	void SetBoundingBox(Vector3f min, Vector3f max);
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, Ref<Material> material);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<Submesh>& submeshes, const std::vector<Ref<Material>>& materials);
	virtual ~Mesh() = default;

	Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
	Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

	const std::vector<Vertex>& GetVertexList()const { return m_Vertices; }
	const std::vector<uint32_t>& GetIndexList()const { return m_Indices; }
	std::vector<Vertex>& GetVertexList() { return m_Vertices; }
	std::vector<uint32_t>& GetIndexList() { return m_Indices; }

	const std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }

	const BoundingBox& GetBounds() const { return m_Bounds; }
	void SetBounds(const BoundingBox& bb) { m_Bounds = bb; }
	void SetMaterials(const std::vector<Ref<Material>> materials) { m_Materials = materials; }
	std::vector<Ref<Material>> GetMaterials() const { return m_Materials; }

	uint32_t GetIndexCount() { return (uint32_t)m_Indices.size(); }
private:
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;
	std::vector<Submesh> m_Submeshes;
	std::vector<Ref<Material>> m_Materials;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	BoundingBox m_Bounds;
};