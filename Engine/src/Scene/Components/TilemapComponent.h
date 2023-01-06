#pragma once

#include "Renderer/Tileset.h"
#include "Core/Colour.h"
#include "Utilities/FileUtils.h"
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

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint, tilesWide, tilesHigh, tiles, orientation, isTrigger);

		std::string relativePath;
		if (tileset && !tileset->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(tileset->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint, tilesWide, tilesHigh, tiles, orientation, isTrigger);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
		{
			tileset = AssetManager::GetAsset<Tileset>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			tileset.reset();
		}

		Rebuild();
		runtimeBody = nullptr;
	}
};