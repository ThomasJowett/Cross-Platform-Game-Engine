#include "Astar.h"

#include <queue>
#include <limits>

namespace Astar
{
GridCoord Heuristic::GetDelta(GridCoord source, GridCoord goal)
{
	return GridCoord(abs(source.x - goal.x), abs(source.y - goal.y));
}

uint32_t Heuristic::Manhattan(GridCoord source, GridCoord goal)
{
	GridCoord delta = GetDelta(source, goal);
	return static_cast<uint32_t>(10 * (delta.x + delta.y));
}

uint32_t Heuristic::Euclidean(GridCoord source, GridCoord goal)
{
	GridCoord delta = GetDelta(source, goal);
	return static_cast<uint32_t>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
}

uint32_t Heuristic::Octagonal(GridCoord source, GridCoord goal)
{
	GridCoord delta = GetDelta(source, goal);
	return static_cast <uint32_t>(10 * (delta.x + delta.y) + (-6) * ((delta.x < delta.y) ? delta.x : delta.y));
}

namespace
{
	constexpr uint32_t c_StraightCost = 10;
	constexpr uint32_t c_DiagonalCost = 14;

	// First 4 are orthogonal, last 4 diagonal
	const GridCoord c_Directions[8] = {
		{ 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
		{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
	};

	struct OpenEntry
	{
		uint32_t F;
		uint32_t H;
		size_t index;

		// Min-heap on F, ties broken towards the goal
		bool operator > (const OpenEntry& other) const
		{
			return F != other.F ? F > other.F : H > other.H;
		}
	};
}

std::vector<GridCoord> FindPath(const AstarGrid& grid, GridCoord source, GridCoord goal, bool diagonalMovement, const HeuristicFunction& heuristic)
{
	std::vector<GridCoord> path;

	if (grid.DetectCollision(source) || grid.DetectCollision(goal))
		return path;

	const size_t cellCount = (size_t)grid.width * (size_t)grid.height;
	auto toIndex = [&grid](GridCoord c) { return (size_t)c.y * (size_t)grid.width + (size_t)c.x; };
	auto toCoord = [&grid](size_t i) { return GridCoord((int)(i % grid.width), (int)(i / grid.width)); };

	constexpr size_t noParent = std::numeric_limits<size_t>::max();
	std::vector<uint32_t> gScore(cellCount, std::numeric_limits<uint32_t>::max());
	std::vector<size_t> parent(cellCount, noParent);
	std::vector<bool> closed(cellCount, false);

	std::priority_queue<OpenEntry, std::vector<OpenEntry>, std::greater<OpenEntry>> open;

	const size_t sourceIndex = toIndex(source);
	const size_t goalIndex = toIndex(goal);
	gScore[sourceIndex] = 0;
	uint32_t sourceH = heuristic(source, goal);
	open.push({ sourceH, sourceH, sourceIndex });

	const int directionCount = diagonalMovement ? 8 : 4;
	bool found = false;

	while (!open.empty())
	{
		size_t currentIndex = open.top().index;
		open.pop();

		// Stale entry left behind by a cheaper re-push
		if (closed[currentIndex])
			continue;

		if (currentIndex == goalIndex)
		{
			found = true;
			break;
		}

		closed[currentIndex] = true;
		GridCoord current = toCoord(currentIndex);

		for (int i = 0; i < directionCount; i++)
		{
			const GridCoord& direction = c_Directions[i];
			GridCoord neighbour = current + direction;

			if (grid.DetectCollision(neighbour))
				continue;

			bool isDiagonal = i >= 4;

			// No cutting corners past a blocked cell
			if (isDiagonal && (grid.DetectCollision({ current.x + direction.x, current.y })
				|| grid.DetectCollision({ current.x, current.y + direction.y })))
				continue;

			size_t neighbourIndex = toIndex(neighbour);
			if (closed[neighbourIndex])
				continue;

			uint32_t tentativeG = gScore[currentIndex] + (isDiagonal ? c_DiagonalCost : c_StraightCost);
			if (tentativeG < gScore[neighbourIndex])
			{
				gScore[neighbourIndex] = tentativeG;
				parent[neighbourIndex] = currentIndex;
				uint32_t h = heuristic(neighbour, goal);
				open.push({ tentativeG + h, h, neighbourIndex });
			}
		}
	}

	if (!found)
		return path;

	for (size_t index = goalIndex; index != noParent; index = parent[index])
		path.push_back(toCoord(index));

	std::reverse(path.begin(), path.end());
	return path;
}

std::vector<Vector2f> FindPath(const AstarGrid& grid, Vector2f source, Vector2f goal, bool diagonalMovement, const HeuristicFunction& heuristic)
{
	std::vector<Vector2f> path;

	GridCoord sourceCoords;
	GridCoord goalCoords;

	if (!grid.PositionToGridCoord(source, sourceCoords) || !grid.PositionToGridCoord(goal, goalCoords))
		return path;

	std::vector<GridCoord> cells = FindPath(grid, sourceCoords, goalCoords, diagonalMovement, heuristic);
	path.reserve(cells.size());
	for (const GridCoord& cell : cells)
	{
		Vector2f position;
		grid.GridCoordToPosition(cell, position);
		path.push_back(position);
	}
	return path;
}
}
