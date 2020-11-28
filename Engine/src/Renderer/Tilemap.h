#pragma once

#include <filesystem>

#include "cereal/access.hpp"

#include "Core/Colour.h"

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

	uint32_t m_Width, m_Hieght;
	uint32_t m_TileWidth, m_TileHeight;

	uint32_t m_HexSideLength;
	StaggerAxis m_StaggerAxis;
	StaggerIndex m_StaggerIndex;

	Colour m_BackgroundColour;

	bool m_Infinite;

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

