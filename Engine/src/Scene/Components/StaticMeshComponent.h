#pragma once

#include "Core/core.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

struct StaticMeshComponent
{
	Ref<Mesh> mesh;
	Ref<Material> material;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent&) = default;
	StaticMeshComponent(Ref<Mesh> mesh, Ref<Material> material)
		:mesh(mesh), material(material) {}

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const 
	{
		std::string relativeMeshPath;
		std::string relativeMaterialPath;
		if (mesh)
		{
			if (!mesh->GetFilepath().empty())
				relativeMeshPath = FileUtils::RelativePath(mesh->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		}

		if (material)
		{
			if (!material->GetFilepath().empty())
				relativeMaterialPath = FileUtils::RelativePath(material->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		}

		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("Material", relativeMaterialPath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativeMeshPath;
		std::string relativeMaterialPath;
		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("Material", relativeMaterialPath));

		if (!relativeMeshPath.empty())
		{
			mesh = CreateRef<Mesh>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativeMeshPath));
		}
		else
		{
			mesh = nullptr;
		}

		if (!relativeMaterialPath.empty())
		{
			material = CreateRef<Material>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativeMaterialPath));
		}
		else
		{
			material = nullptr;
		}
	}
};
