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

	void SetMesh(const Ref<StaticMesh> newMesh)
	{
		mesh = newMesh;
		materialOverrides.resize(mesh->GetMesh()->GetSubmeshes().size());

		const auto& materials = mesh->GetMesh()->GetMaterials();

		for (size_t i = 0; i < mesh->GetMesh()->GetSubmeshes().size(); ++i)
		{
			materialOverrides[i] = materials[mesh->GetMesh()->GetSubmeshes()[i].materialIndex];
		}
	}

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativeMeshPath;
		std::vector<std::string> relativeMaterials;
		if (mesh)
		{
			if (!mesh->GetFilepath().empty())
				relativeMeshPath = mesh->GetFilepath().string();
			
			for (size_t i = 0; i < mesh->GetMesh()->GetSubmeshes().size(); ++i)
			{
				relativeMaterials.push_back(materialOverrides[i]->GetFilepath().string());
			}
			
		}
		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("MaterialOverrides", relativeMaterials));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativeMeshPath;
		std::vector<std::string> relativeMaterials;
		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("MaterialOverrides", relativeMaterials));

		if (!relativeMeshPath.empty() && relativeMeshPath != "#")
		{
			mesh = AssetManager::GetAsset<StaticMesh>(relativeMeshPath);
			
			materialOverrides.resize(mesh->GetMesh()->GetSubmeshes().size());
			for (size_t i = 0; i < mesh->GetMesh()->GetSubmeshes().size(); ++i)
			{
				if (i < relativeMaterials.size() && !relativeMaterials[i].empty())
					materialOverrides[i] = AssetManager::GetAsset<Material>(relativeMaterials[i]);
				else
				{
					auto matIndex = mesh->GetMesh()->GetSubmeshes()[i].materialIndex;
					materialOverrides[i] = mesh->GetMesh()->GetMaterials()[matIndex];
				}
			}
		}
		else
		{
			mesh.reset();
			materialOverrides.clear();
		}
	}
};
