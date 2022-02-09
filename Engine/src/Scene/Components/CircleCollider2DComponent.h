#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

struct CircleCollider2DComponent
{
	Vector2f Offset = { 0.0f,0.0f };

	float Radius = 0.5f;

	//TODO: create physics material
	float Density = 1.0f;
	float Friction = 0.5f;
	float Restitution = 0.0f;

	void* RuntimeFixture = nullptr;

	CircleCollider2DComponent() = default;
	CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Offset", Offset));
		archive(cereal::make_nvp("Radius", Radius));
		archive(cereal::make_nvp("Density", Density));
		archive(cereal::make_nvp("Friction", Friction));
		archive(cereal::make_nvp("Restitution", Restitution));
	}
};