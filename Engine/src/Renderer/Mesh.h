#pragma once

#include "VertexArray.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "Utilities/FileUtils.h"
#include "Core/Application.h"
#include "Core/Asset.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "Core/BoundingBox.h"
#include "Material.h"

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

class Mesh
{
public:
	Mesh() = default;
	explicit Mesh(Ref<VertexArray> vertexArray);
	explicit Mesh(Ref<VertexArray> vertexArray, Ref<Material> material);
	virtual ~Mesh() = default;
	void LoadModel(Ref<VertexArray> vertexArray);

	Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
	const BoundingBox& GetBounds() const { return m_Bounds; }
	void SetBounds(const BoundingBox& bb) { m_Bounds = bb; }
	void SetMaterial(Ref<Material> material) { m_Material = material; }
	Ref<Material> GetMaterial() const { return m_Material; }
private:
	Ref<VertexArray> m_VertexArray;
	BoundingBox m_Bounds;
	Ref<Material> m_Material;
};