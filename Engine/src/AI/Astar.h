#pragma once

#include <vector>
#include <functional>
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

		// Row-major, width * height, true = blocked
		std::vector<bool> collisions;

		AstarGrid(int width, int height, float cellWidth = 1.0f, float cellHeight = 1.0f, Vector2f origin = Vector2f())
			:width(width), height(height), cellWidth(cellWidth), cellHeight(cellHeight), origin(origin),
			collisions((size_t)std::max(width, 0) * (size_t)std::max(height, 0), false) {}

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

		void SetBlocked(GridCoord coordinate, bool blocked)
		{
			if (InBounds(coordinate))
				collisions[Index(coordinate)] = blocked;
		}

		// Out of bounds counts as blocked
		bool DetectCollision(GridCoord coordinate) const
		{
			if (!InBounds(coordinate))
				return true;
			return collisions[Index(coordinate)];
		}

	private:
		size_t Index(GridCoord coordinate) const { return (size_t)coordinate.y * (size_t)width + (size_t)coordinate.x; }
	};

	using HeuristicFunction = std::function<uint32_t(GridCoord, GridCoord)>;

	class Heuristic
	{
		static GridCoord GetDelta(GridCoord source, GridCoord goal);

	public:
		static uint32_t Manhattan(GridCoord source, GridCoord goal);
		static uint32_t Euclidean(GridCoord source, GridCoord goal);
		static uint32_t Octagonal(GridCoord source, GridCoord goal);
	};

	// Path runs source -> goal inclusive; empty if either end is blocked or the goal is unreachable
	std::vector<GridCoord> FindPath(const AstarGrid& grid, GridCoord source, GridCoord goal,
		bool diagonalMovement = true, const HeuristicFunction& heuristic = &Heuristic::Octagonal);

	// As above, returning cell centres in world space
	std::vector<Vector2f> FindPath(const AstarGrid& grid, Vector2f source, Vector2f goal,
		bool diagonalMovement = true, const HeuristicFunction& heuristic = &Heuristic::Octagonal);
}
