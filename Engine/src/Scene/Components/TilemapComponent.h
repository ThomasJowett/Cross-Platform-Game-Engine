#pragma once

#include "Renderer/Tileset.h"
#include "Core/Colour.h"

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

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(cereal::make_nvp("Tint", tint));
		archive(cereal::make_nvp("Tiles Wide", tilesWide));
		archive(cereal::make_nvp("Tiles High", tilesHigh));
		archive(cereal::make_nvp("Tiles", tiles));

		std::string relativePath;
		if (tileset && !tileset->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(tileset->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(cereal::make_nvp("Filepath", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(cereal::make_nvp("Tint", tint));
		archive(cereal::make_nvp("Tiles Wide", tilesWide));
		archive(cereal::make_nvp("Tiles High", tilesHigh));
		archive(cereal::make_nvp("Tiles", tiles));
		std::string relativePath;
		archive(cereal::make_nvp("Filepath", relativePath));
		if (!relativePath.empty())
		{
			tileset = CreateRef<Tileset>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			tileset = nullptr;
		}
	}
};