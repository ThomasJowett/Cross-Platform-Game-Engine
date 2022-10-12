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
		}

		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("MaterialOverrides", materialOverrides));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativeMeshPath;
		archive(cereal::make_nvp("Mesh", relativeMeshPath));
		archive(cereal::make_nvp("Material", materialOverrides));

		if (!relativeMeshPath.empty())
		{
			mesh = AssetManager::GetStaticMesh(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativeMeshPath));
			materialOverrides.resize(mesh->GetMeshes().size());
		}
		else
		{
			mesh.reset();
			materialOverrides.clear();
		}
	}
};
