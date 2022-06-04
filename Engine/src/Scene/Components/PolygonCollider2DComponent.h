#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

#include <vector>

struct PolygonCollider2DComponent
{
	std::vector<Vector2f> vertices = 
	{
		{0.0f, 1.0f},
		{-0.9510565f, 0.309017f},
		{-0.5877852f, -0.8090171f},
		{0.5877854f,-0.8090169f},
		{0.9510565f, 0.3090171f}
	};

	Vector2f offset;

	//TODO: create physics material
	float density = 1.0f;
	float friction = 0.5f;
	float restitution = 0.0f;

	void* runtimeFixture = nullptr;

	PolygonCollider2DComponent() = default;
	PolygonCollider2DComponent(const PolygonCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Vertices", vertices));
		archive(cereal::make_nvp("Offset", offset));
		archive(cereal::make_nvp("Density", density));
		archive(cereal::make_nvp("Friction", friction));
		archive(cereal::make_nvp("Restitution", restitution));
	}
};
