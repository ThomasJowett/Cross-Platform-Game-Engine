#pragma once

#include "Asset/Tileset.h"
#include "Core/Colour.h"
#include "Utilities/FileUtils.h"
#include "Utilities/SerializationUtils.h"
#include "Core/Application.h"
#include "Scene/AssetManager.h"
#include "Renderer/Mesh.h"
#include "Scripting/Lua/LuaBindings.h"

class b2Body;

LUA_TYPE_NAME(Tileset, "Tileset")

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

	REFLECT_LUA_BEGIN(TilemapComponent)
		REFLECT_LUA_PROPERTY(tileset, "The tileset this map draws its tiles from")
		REFLECT_LUA_PROPERTY(tint, "Colour multiplied over every tile")
		REFLECT_LUA_PROPERTY(tilesWide, "Number of tile columns - use Resize() to change this and the tile grid together")
		REFLECT_LUA_PROPERTY(tilesHigh, "Number of tile rows - use Resize() to change this and the tile grid together")
		REFLECT_LUA_PROPERTY(tileWidth, "Width of a single tile, in pixels")
		REFLECT_LUA_PROPERTY(tileHeight, "Height of a single tile, in pixels")
		REFLECT_LUA_PROPERTY(orientation, "How tiles are laid out: orthogonal, isometric, staggered or hexagonal")
		REFLECT_LUA_PROPERTY(isTrigger, "Whether this tilemap's collision only reports overlaps instead of physically blocking")
		REFLECT_LUA_FUNCTION(Rebuild, "Regenerate the tilemap's mesh from the current tile grid")

		REFLECT_LUA_FUNCTION_CUSTOM("SetTile", "Set the tile index at the given column/row, if within bounds", [](Self& c, uint32_t x, uint32_t y, uint32_t tileId) {
			if (y < c.tiles.size() && x < c.tiles[y].size()) {
				c.tiles[y][x] = tileId;
			}
		});

		REFLECT_LUA_FUNCTION_CUSTOM("GetTile", "Get the tile index at the given column/row, or 0 if out of bounds", [](Self& c, uint32_t x, uint32_t y) -> uint32_t {
			if (y < c.tiles.size() && x < c.tiles[y].size()) {
				return c.tiles[y][x];
			}
			return 0;
		});

		REFLECT_LUA_FUNCTION_CUSTOM("Resize", "Resize the tile grid to the given width/height in tiles, preserving existing tile data", [](Self& c, uint32_t width, uint32_t height) {
			c.tilesWide = width;
			c.tilesHigh = height;
			c.tiles.resize(height);
			for (auto& row : c.tiles) {
				row.resize(width);
			}
		});
	REFLECT_LUA_END()

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
