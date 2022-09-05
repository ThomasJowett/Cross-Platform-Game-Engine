#pragma once

#include "Core/Asset.h"

class PhysicsMaterial : public Asset
{
public:
	PhysicsMaterial() = default;
	PhysicsMaterial(const std::filesystem::path& filepath);
	PhysicsMaterial(float density, float friction, float restitution);
	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;

	bool SaveAs(const std::filesystem::path& filepath) const;
	bool Save() const;

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }

	float GetDensity() { return m_Density; }
	void SetDensity(float density) { m_Density = density; }

	float GetFriction() { return m_Friction; }
	void SetFriction(float friction) { m_Friction = friction; }

	float GetRestitution() { return m_Restitution; }
	void SetRestitution(float restitution) { m_Restitution; }

private:
	std::filesystem::path m_Filepath;

	float m_Density = 1.0f;
	float m_Friction = 0.5f;
	float m_Restitution = 0.0f;
};