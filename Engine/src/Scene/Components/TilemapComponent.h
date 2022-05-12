#pragma once

#include "Renderer/Tilemap.h"
#include "Renderer/Tileset.h"

struct TilemapComponent
{
	enum class Orientation
	{
		orthogonal,
		isometric,
		staggered,
		hexagonal
	};

	Tileset tileset;

	uint32_t** tiles;
	uint32_t tilesWide = 0;
	uint32_t tilesHigh = 0;

	Orientation orientation = Orientation::orthogonal;

	TilemapComponent() = default;
	TilemapComponent(const TilemapComponent&) = default;
	TilemapComponent(Orientation orientation, uint32_t tilesWide, uint32_t tilesHigh)
		:orientation(orientation), tilesWide(tilesWide), tilesHigh(tilesHigh) 
	{
		tiles = new uint32_t * [tilesHigh];

		for (uint32_t i = 0; i < tilesHigh; i++)
		{
			tiles[i] = new uint32_t[tilesWide];
		}
	}

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("Tiles Wide", tilesWide));
		archive(cereal::make_nvp("Tiles High", tilesHigh));
		std::vector<std::vector<uint32_t>> arr;
		for (size_t i = 0; i < tilesHigh; i++)
		{
			arr.push_back(std::vector<uint32_t>());
			arr[i].assign(tiles[i], tiles[i] + tilesWide);
		}
		archive(arr);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("Tiles Wide", tilesWide));
		archive(cereal::make_nvp("Tiles High", tilesHigh));
		std::vector<std::vector<uint32_t>> arr;
		archive(arr);
		tiles = new uint32_t * [tilesHigh];
		for (size_t i = 0; i < tilesHigh; i++)
		{
			tiles[i] = arr[i].data();
		}
	}
};