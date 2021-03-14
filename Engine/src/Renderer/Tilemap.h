#pragma once

#include <filesystem>

#include "cereal/access.hpp"

#include "Core/Colour.h"
#include "Renderer/Texture.h"


class Tileset
{
public:
	Tileset() = default;

	bool Load(const std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath);

private:
	std::string m_Name;
	Ref<Texture2D> m_Texture;

	uint32_t m_TileWidth;
	uint32_t m_TileHeight;

	uint32_t m_TileCount;
	uint32_t m_Columns;

	Colour m_BackgroundColour;

	uint32_t m_OffsetX, m_OffsetY;

	std::filesystem::path m_Filepath;
};

class Tilemap
{
	enum class Orientation
	{
		orthogonal,
		isometric,
		staggered,
		hexagonal
	};

	enum class RenderingOrder
	{
		rightDown,
		rightUp,
		leftDown,
		leftUp
	};

	enum class StaggerAxis
	{
		X,
		Y
	};

	enum class StaggerIndex
	{
		even,
		odd
	};

public:
	Tilemap() = default;
	bool Load(std::filesystem::path);
	bool Save(std::filesystem::path);

private:
	std::string m_Name;
	std::filesystem::path m_Filepath;

	Orientation m_Orientation;
	RenderingOrder m_RenderingOrder;

	bool m_IsStaggered;

	uint32_t m_Width, m_Height;
	uint32_t m_TileWidth, m_TileHeight;

	uint32_t m_HexSideLength;
	StaggerAxis m_StaggerAxis;
	StaggerIndex m_StaggerIndex;

	Colour m_BackgroundColour;

	bool m_Infinite;

	std::vector<std::pair<Tileset, uint32_t>> m_Tilesets;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive)
	{
	}

	template<typename Archive>
	void load(Archive& archive)
	{
	}
};

