#pragma once

#include "Renderer/Tilemap.h"

struct TilemapComponent
{
	Tilemap tilemap;

	TilemapComponent() = default;
	TilemapComponent(const TilemapComponent&) = default;
	TilemapComponent(const Tilemap& tilemap)
		:tilemap(tilemap) {}

	operator Tilemap& () { return tilemap; }
	operator const Tilemap& () const { return tilemap; }
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Tilemap", tilemap));
	}
};