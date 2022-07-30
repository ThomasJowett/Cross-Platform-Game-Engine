#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

struct BoxCollider2DComponent
{
	Vector2f offset = { 0.0f, 0.0f };
	Vector2f size = { 0.5f, 0.5f };

	//TODO: create physics material
	float density = 1.0f;
	float friction = 0.5f;
	float restitution = 0.0f;

	void* RuntimeFixture = nullptr;

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Offset", offset));
		archive(cereal::make_nvp("Size", size));
		archive(cereal::make_nvp("Density", density));
		archive(cereal::make_nvp("Friction", friction));
		archive(cereal::make_nvp("Restitution", restitution));
	}
};