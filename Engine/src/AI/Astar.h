#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include "math/Vector2f.h"

namespace Astar
{
	struct GridCoord
	{
		int x;
		int y;

		GridCoord() :x(0), y(0) {}
		GridCoord(int x, int y) :x(x), y(y) {}

		bool operator == (const GridCoord& other) const
		{
			return (x == other.x && y == other.y);
		}

		bool operator != (const GridCoord& other) const
		{
			return !(*this == other);
		}

		GridCoord operator + (const GridCoord& other) const
		{
			return { x + other.x, y + other.y };
		}

		GridCoord operator - (const GridCoord& other) const
		{
			return { x - other.x, y - other.y };
		}
	};

	// Cell (x, y) covers [origin + (x, y) * cellSize, origin + (x + 1, y + 1) * cellSize)
	struct AstarGrid
	{
		int width;
		int height;
		float cellWidth;
		float cellHeight;

		Vector2f origin;

		// Row-major, width * height, non-zero = blocked
		std::vector<uint8_t> collisions;

		AstarGrid(int width, int height, float cellWidth = 1.0f, float cellHeight = 1.0f, Vector2f origin = Vector2f())
			:width(width), height(height), cellWidth(cellWidth), cellHeight(cellHeight), origin(origin),
			collisions((size_t)std::max(width, 0) * (size_t)std::max(height, 0), 0) {}

		bool InBounds(GridCoord coordinate) const
		{
			return coordinate.x >= 0 && coordinate.y >= 0 && coordinate.x < width && coordinate.y < height;
		}

		bool PositionToGridCoord(Vector2f position, GridCoord& coordinate) const
		{
			Vector2f local = position - origin;
			coordinate.x = (int)std::floor(local.x / cellWidth);
			coordinate.y = (int)std::floor(local.y / cellHeight);
			return InBounds(coordinate);
		}

		// Returns the centre of the cell
		bool GridCoordToPosition(GridCoord coordinate, Vector2f& position) const
		{
			position.x = ((float)coordinate.x + 0.5f) * cellWidth;
			position.y = ((float)coordinate.y + 0.5f) * cellHeight;
			position += origin;
			return InBounds(coordinate);
		}

		// Invalidates region labels
		void SetBlocked(GridCoord coordinate, bool blocked)
		{
			if (InBounds(coordinate))
			{
				collisions[Index(coordinate)] = blocked ? 1 : 0;
				m_Regions.clear();
			}
		}

		// Out of bounds counts as blocked
		bool DetectCollision(GridCoord coordinate) const
		{
			if (!InBounds(coordinate))
				return true;
			return collisions[Index(coordinate)] != 0;
		}

		// Flood fills 4-connected open areas; diagonals can't cut corners so this matches 8-way movement too
		void LabelRegions();

		bool HasRegions() const { return !m_Regions.empty(); }

		// 0 for blocked cells, otherwise the region id; only valid while HasRegions()
		uint32_t GetRegion(GridCoord coordinate) const { return m_Regions[Index(coordinate)]; }

		size_t Index(GridCoord coordinate) const { return (size_t)coordinate.y * (size_t)width + (size_t)coordinate.x; }

	private:
		std::vector<uint32_t> m_Regions;
	};

	enum class HeuristicType
	{
		Manhattan,
		Euclidean,
		Octagonal
	};

	class Heuristic
	{
		static GridCoord GetDelta(GridCoord source, GridCoord goal);

	public:
		static uint32_t Manhattan(GridCoord source, GridCoord goal);
		static uint32_t Euclidean(GridCoord source, GridCoord goal);
		static uint32_t Octagonal(GridCoord source, GridCoord goal);
	};

	// Path runs source -> goal inclusive; empty if either end is blocked or the goal is unreachable.
	// Fails immediately across regions when the grid has been labelled
	std::vector<GridCoord> FindPath(const AstarGrid& grid, GridCoord source, GridCoord goal,
		bool diagonalMovement = true, HeuristicType heuristic = HeuristicType::Octagonal);

	// As above, returning cell centres in world space
	std::vector<Vector2f> FindPath(const AstarGrid& grid, Vector2f source, Vector2f goal,
		bool diagonalMovement = true, HeuristicType heuristic = HeuristicType::Octagonal);
}
