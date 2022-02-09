#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

struct BoxCollider2DComponent
{
	Vector2f Offset = { 0.0f, 0.0f };
	Vector2f Size = { 0.5f, 0.5f };

	//TODO: create physics material
	float Density = 1.0f;
	float Friction = 0.5f;
	float Restitution = 0.0f;

	void* RuntimeFixture = nullptr;

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Offset", Offset));
		archive(cereal::make_nvp("Size", Size));
		archive(cereal::make_nvp("Density", Density));
		archive(cereal::make_nvp("Friction", Friction));
		archive(cereal::make_nvp("Restitution", Restitution));
	}
};