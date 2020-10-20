#pragma once

#include "math/Vector3f.h"

#include "cereal/cereal.hpp"

struct TransformComponent
{
	Vector3f Position = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f Rotation = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f Scale = Vector3f(1.0f, 1.0f, 1.0f);

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const Vector3f & position)
		:Position(position) {}
	TransformComponent(const Vector3f& position, const Vector3f& rotation, const Vector3f& scale)
		:Position(position), Rotation(rotation), Scale(scale) {}
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Position", Position));
		archive(cereal::make_nvp("Rotation", Rotation));
		archive(cereal::make_nvp("Scale", Scale));
	}
};