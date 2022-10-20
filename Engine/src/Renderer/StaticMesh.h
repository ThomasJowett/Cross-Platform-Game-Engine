#pragma once

#include "Core/Asset.h"
#include "Mesh.h"

class StaticMesh : public Asset
{
public:
	StaticMesh() = default;
	StaticMesh(const std::filesystem::path& filepath);
	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual const std::filesystem::path& GetFilepath() const override;
	void AddMesh(Ref<Mesh> mesh) { m_Meshes.push_back(mesh); }

	const std::vector<Ref<Mesh>> GetMeshes() const { return m_Meshes; }

private:

	std::vector<Ref<Mesh>> m_Meshes;
};
