#pragma once

#include "Core/core.h"
#include "Asset/StaticMesh.h"
#include "Asset/Material.h"
#include "Scene/AssetManager.h"

struct StaticMeshComponent
{
	Ref<StaticMesh> mesh;
	std::vector<Ref<Material>> materialOverrides;

	StaticMeshComponent() = default;
	StaticMeshComponent(const StaticMeshComponent&) = default;
	StaticMeshComponent(Ref<StaticMesh> mesh)
		:mesh(mesh) {}

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativeMeshPath;
		if (mesh)
		{
			if (!mesh->GetFilepath().empty())
				relativeMeshPath = mesh->GetFilepath().string();
			std::vector<std::string> relativeMaterials;
			for (size_t i = 0; i < mesh->GetMesh()->GetSubmeshes().size(); ++i)
			{
				relativeMaterials.push_back(materialOverrides[i]->GetFilepath().string());
			}
			archive(cereal::make_nvp("MaterialOverrides", relativeMaterials));
		}

		archive(cereal::make_nvp("Mesh", relativeMeshPath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativeMeshPath;
		std::vector<std::string> relativeMaterials;
		archive(cereal::make_nvp("Mesh", relativeMeshPath));

		if (!relativeMeshPath.empty())
		{
			mesh = AssetManager::GetAsset<StaticMesh>(relativeMeshPath);
			archive(cereal::make_nvp("MaterialOverrides", relativeMaterials));
			materialOverrides.resize(mesh->GetMesh()->GetSubmeshes().size());
			for (size_t i = 0; i < mesh->GetMesh()->GetSubmeshes().size(); ++i)
			{
				if (relativeMaterials[i].empty())
					materialOverrides[i] = mesh->GetMesh()->GetMaterials()[mesh->GetMesh()->GetSubmeshes()[i].materialIndex];
				else
					materialOverrides[i] = AssetManager::GetAsset<Material>(relativeMaterials[i]);
			}
		}
		else
		{
			mesh.reset();
			materialOverrides.clear();
		}
	}
};
