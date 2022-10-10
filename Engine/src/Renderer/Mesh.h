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

class Mesh : public Asset
{
public:
	Mesh() = default;
	explicit Mesh(const std::filesystem::path& filepath);
	explicit Mesh(Ref<VertexArray> vertexArray);
	virtual ~Mesh() = default;
	void LoadModel(Ref<VertexArray> vertexArray);
	bool Load(const std::filesystem::path& filepath) override;

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }

	Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
private:
	Ref<VertexArray> m_VertexArray;
	AABB aabb;
};