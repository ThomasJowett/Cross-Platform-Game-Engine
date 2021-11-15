#pragma once

#include "Core/Colour.h"

#include "cereal/cereal.hpp"

#include "Renderer/Texture.h"
#include "Renderer/Material.h"

struct SpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Material material;
	float tilingFactor = 1.0f;

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Tint", tint));
		archive(cereal::make_nvp("Material", material));
		archive(cereal::make_nvp("Tiling Factor", tilingFactor));
	}
};
