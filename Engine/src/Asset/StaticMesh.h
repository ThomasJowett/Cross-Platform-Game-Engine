#pragma once

#include "Core/Asset.h"
#include "Renderer/Mesh.h"

class StaticMesh : public Asset
{
public:
	StaticMesh() = default;
	StaticMesh(const std::filesystem::path& filepath);
	StaticMesh(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;

	const BoundingBox& GetBounds() { return m_Mesh->GetBounds(); }

	const Ref<Mesh>& GetMesh() const { return m_Mesh; }

private:
	Ref<Mesh> m_Mesh;
};
