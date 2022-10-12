#pragma once

#include "VertexArray.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "Utilities/FileUtils.h"
#include "Core/Application.h"
#include "Core/Asset.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "Core/AABB.h"
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
	virtual ~Mesh() = default;
	void LoadModel(Ref<VertexArray> vertexArray);

	Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
	const AABB& GetBounds() const { return m_Bounds; }
	void SetMaterial(Ref<Material> material) { m_Material = material; }
	Ref<Material> GetMaterial() const { return m_Material; }
private:
	Ref<VertexArray> m_VertexArray;
	AABB m_Bounds;
	Ref<Material> m_Material;
};