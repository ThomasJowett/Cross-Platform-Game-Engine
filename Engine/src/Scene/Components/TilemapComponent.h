#pragma once

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

	Ref<Tileset> tileset;
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };

	uint32_t** tiles = nullptr;
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
		archive(cereal::make_nvp("Tint", tint));
		archive(cereal::make_nvp("Tiles Wide", tilesWide));
		archive(cereal::make_nvp("Tiles High", tilesHigh));
		std::vector<std::vector<uint32_t>> arr;
		for (size_t i = 0; i < tilesHigh; i++)
		{
			arr.push_back(std::vector<uint32_t>());
			arr[i].assign(tiles[i], tiles[i] + tilesWide);
		}
		archive(cereal::make_nvp("Tiles", arr));

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
		std::vector<std::vector<uint32_t>> arr;
		archive(cereal::make_nvp("Tiles", arr));
		tiles = new uint32_t * [tilesHigh];
		for (size_t i = 0; i < tilesHigh; i++)
		{
			tiles[i] = new uint32_t[tilesWide];
			std::memcpy(tiles[i], arr[i].data(), tilesWide * sizeof(uint32_t));
		}

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