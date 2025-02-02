#pragma once

#include "Asset/Tileset.h"
#include "Core/Colour.h"
#include "Utilities/FileUtils.h"
#include "Utilities/SerializationUtils.h"
#include "Core/Application.h"
#include "Scene/AssetManager.h"
#include "Renderer/Mesh.h"

class b2Body;

struct TilemapComponent
{
	enum class Orientation
	{
		orthogonal,
		isometric,
		staggered,
		hexagonal
	};

	Ref<Tileset> tileset;
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };

	std::vector<std::vector<uint32_t>> tiles;
	uint32_t tilesWide = 0;
	uint32_t tilesHigh = 0;

	uint32_t tileWidth = 32;
	uint32_t tileHeight = 32;

	Orientation orientation = Orientation::orthogonal;

	bool isTrigger = false;

	Ref<Mesh> mesh;

	b2Body* runtimeBody = nullptr;

	TilemapComponent() = default;
	TilemapComponent(const TilemapComponent&) = default;
	TilemapComponent(Orientation orientation, uint32_t tilesWide, uint32_t tilesHigh)
		:orientation(orientation), tilesWide(tilesWide), tilesHigh(tilesHigh),
		tiles(tilesHigh)
	{
		for (auto& row : tiles)
		{
			row.resize(tilesWide);
		}
	}

	void Rebuild();

	Vector2f IsoToWorld(uint32_t x, uint32_t y) const;
	Vector2f WorldToIso(Vector2f v) const;

	Vector2f HexToWorld(uint32_t q, uint32_t r) const;
	Vector2f WorldToHex(Vector2f v) const;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint, tilesWide, tilesHigh, tiles, tileWidth, tileHeight, orientation, isTrigger);

		SerializationUtils::SaveAssetToArchive(archive, tileset);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint, tilesWide, tilesHigh, tiles, tileWidth, tileHeight, orientation, isTrigger);
		SerializationUtils::LoadAssetFromArchive(archive, tileset);

		Rebuild();
		runtimeBody = nullptr;
	}
};