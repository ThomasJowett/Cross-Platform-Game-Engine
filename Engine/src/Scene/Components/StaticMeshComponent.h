#pragma once

#include "Core/core.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

struct StaticMeshComponent
{
	Mesh Geometry;
	Material material;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent&) = default;
	StaticMeshComponent(const Mesh& mesh, const Material& material)
		:Geometry(mesh),material(material) {}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Mesh", Geometry));
		archive(cereal::make_nvp("Material", material));
	}
};
