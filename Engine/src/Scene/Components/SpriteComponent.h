#pragma once

#include "Core/Colour.h"

struct SpriteComponent
{
	Colour Tint{ 1.0f, 1.0f,1.0f,1.0f };

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;
	SpriteComponent(const Colour& colour)
		:Tint(colour) {}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Tint", Tint));
	}
};
