#pragma once

#include "Renderer/Tileset.h"
#include "Core/Colour.h"
#include "Utilities/FileUtils.h"
#include "Core/Application.h"
#include "Scene/AssetManager.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Material.h"

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

	Ref<VertexArray> vertexArray;
	Ref<Material> material;

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

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint, tilesWide, tilesHigh, tiles);

		std::string relativePath;
		if (tileset && !tileset->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(tileset->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint, tilesWide, tilesHigh, tiles);
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
	}
};