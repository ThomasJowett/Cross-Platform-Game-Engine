#pragma once

#include "cereal/cereal.hpp"

struct BillboardComponent
{
	enum class Orientation
	{
		WorldUp,
		Camera
	};

	enum class Position
	{
		World,
		Camera
	};

	BillboardComponent() = default;
	BillboardComponent(const BillboardComponent&) = default;

	Orientation orientation = Orientation::Camera;
	Position position = Position::World;

	Vector2f screenPosition;
private:

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(orientation, position, screenPosition);
	}
};