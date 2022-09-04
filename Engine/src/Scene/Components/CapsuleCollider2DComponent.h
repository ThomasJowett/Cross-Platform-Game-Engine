#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

struct CapsuleCollider2DComponent
{
	enum class Direction
	{
		Vertical,
		Horizontal
	}direction;

	Vector2f offset = { 0.0f , 0.0f };

	float radius = 1.0f;
	float height = 1.0f;

	void* RuntimeFixture = nullptr;

	CapsuleCollider2DComponent() = default;
	CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(direction, offset, radius, height);
	}
};
