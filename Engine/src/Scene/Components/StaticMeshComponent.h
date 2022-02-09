#pragma once

#include "Core/core.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

struct StaticMeshComponent
{
	Mesh mesh;
	Material material;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent&) = default;
	StaticMeshComponent(const Mesh& mesh, const Material& material)
		:mesh(mesh),material(material) {}
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Mesh", mesh));
		archive(cereal::make_nvp("Material", material));
	}
};
