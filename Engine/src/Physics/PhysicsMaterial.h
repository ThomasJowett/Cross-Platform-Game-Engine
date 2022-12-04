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

	float GetDensity() { return m_Density; }
	void SetDensity(float density) { m_Density = density; }

	float GetFriction() { return m_Friction; }
	void SetFriction(float friction) { m_Friction = std::clamp(friction, 0.0f, 1.0f); }

	float GetRestitution() { return m_Restitution; }
	void SetRestitution(float restitution) { m_Restitution = std::clamp(restitution, 0.0f, 1.0f);}

	static Ref<PhysicsMaterial> GetDefaultPhysicsMaterial();

private:

	float m_Density = 1.0f;
	float m_Friction = 0.5f;
	float m_Restitution = 0.0f;

	static Ref<PhysicsMaterial> s_DefaultPhysicsMaterial;
};