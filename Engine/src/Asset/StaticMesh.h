#pragma once

#include "Core/Asset.h"
#include "Renderer/Mesh.h"

class StaticMesh : public Asset
{
public:
	StaticMesh() = default;
	StaticMesh(const std::filesystem::path& filepath);
	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;

	const BoundingBox& GetBounds() { return m_Bounds; }
	void SetBounds(const BoundingBox& bounds) { m_Bounds = bounds; }

	const Ref<Mesh>& GetMesh() const { return m_Mesh; }

private:
	Ref<Mesh> m_Mesh;
	BoundingBox m_Bounds;
};
