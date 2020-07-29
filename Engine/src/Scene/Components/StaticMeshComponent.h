#pragma once

#include "Core/core.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

struct MeshComponent
{
	Mesh Geometry;
	Material material;

	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
	MeshComponent(const Mesh& mesh, const Material& material)
		:Geometry(mesh),material(material) {}
};
