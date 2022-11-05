#pragma once

#include "Core/core.h"
#include "Renderer/StaticMesh.h"
#include "Renderer/Material.h"
#include "Scene/AssetManager.h"

struct StaticMeshComponent
{
	Ref<StaticMesh> mesh;
	std::vector<std::string> materialOverrides;

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
				relativeMeshPath = FileUtils::RelativePath(mesh->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
			std::vector<std::string> relativeMaterials;
			for (size_t i = 0; i < mesh->GetMeshes().size(); ++i)
			{
				relativeMaterials.push_back(FileUtils::RelativePath(materialOverrides[i], Application::GetOpenDocumentDirectory()).string());
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
			archive(cereal::make_nvp("MaterialOverrides", relativeMaterials));
			mesh = AssetManager::GetStaticMesh(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativeMeshPath));
			materialOverrides.resize(mesh->GetMeshes().size());
			for (size_t i = 0; i < mesh->GetMeshes().size(); ++i)
			{

				if (relativeMaterials[i].empty())
					materialOverrides[i] = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / mesh->GetMeshes()[i]->GetMaterial()->GetFilepath()).string();
				else
					materialOverrides[i] = relativeMaterials[i];
			}
		}
		else
		{
			mesh.reset();
			materialOverrides.clear();
		}
	}
};
