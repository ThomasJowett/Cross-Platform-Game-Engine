#pragma once

#include "Core/Colour.h"

#include "cereal/cereal.hpp"

#include "Renderer/Texture.h"
#include "Renderer/Material.h"

struct SpriteComponent
{
	Colour Tint{ 1.0f, 1.0f,1.0f,1.0f };
	Material material;
	float TilingFactor = 1.0f;

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;
	SpriteComponent(const Colour& colour, float tilingFactor)
		:Tint(colour),
		TilingFactor(tilingFactor)
	{}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Tint", Tint));
		archive(cereal::make_nvp("Material", material));
		archive(cereal::make_nvp("Tiling Factor", TilingFactor));
	}
};
