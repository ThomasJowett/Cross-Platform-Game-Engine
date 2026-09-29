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

void AstarGrid::LabelRegions()
{
	const size_t cellCount = collisions.size();
	m_Regions.assign(cellCount, 0);

	std::vector<size_t> queue;
	queue.reserve(cellCount);
	uint32_t nextRegion = 1;

	for (size_t seed = 0; seed < cellCount; seed++)
	{
		if (collisions[seed] || m_Regions[seed])
			continue;

		queue.clear();
		queue.push_back(seed);
		m_Regions[seed] = nextRegion;

		for (size_t head = 0; head < queue.size(); head++)
		{
			GridCoord current((int)(queue[head] % width), (int)(queue[head] / width));
			for (int i = 0; i < 4; i++)
			{
				GridCoord neighbour = current + c_Directions[i];
				if (DetectCollision(neighbour))
					continue;

				size_t index = Index(neighbour);
				if (!m_Regions[index])
				{
					m_Regions[index] = nextRegion;
					queue.push_back(index);
				}
			}
		}
		nextRegion++;
	}
}

template<uint32_t(*HeuristicFn)(GridCoord, GridCoord)>
static std::vector<GridCoord> FindPathImpl(const AstarGrid& grid, GridCoord source, GridCoord goal, bool diagonalMovement)
{
	std::vector<GridCoord> path;

	const size_t cellCount = (size_t)grid.width * (size_t)grid.height;
	auto toCoord = [&grid](size_t i) { return GridCoord((int)(i % grid.width), (int)(i / grid.width)); };

	constexpr size_t noParent = std::numeric_limits<size_t>::max();
	std::vector<uint32_t> gScore(cellCount, std::numeric_limits<uint32_t>::max());
	std::vector<size_t> parent(cellCount, noParent);
	std::vector<uint8_t> closed(cellCount, 0);

	std::priority_queue<OpenEntry, std::vector<OpenEntry>, std::greater<OpenEntry>> open;

	const size_t sourceIndex = grid.Index(source);
	const size_t goalIndex = grid.Index(goal);
	gScore[sourceIndex] = 0;
	uint32_t sourceH = HeuristicFn(source, goal);
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

		closed[currentIndex] = 1;
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

			size_t neighbourIndex = grid.Index(neighbour);
			if (closed[neighbourIndex])
				continue;

			uint32_t tentativeG = gScore[currentIndex] + (isDiagonal ? c_DiagonalCost : c_StraightCost);
			if (tentativeG < gScore[neighbourIndex])
			{
				gScore[neighbourIndex] = tentativeG;
				parent[neighbourIndex] = currentIndex;
				uint32_t h = HeuristicFn(neighbour, goal);
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

std::vector<GridCoord> FindPath(const AstarGrid& grid, GridCoord source, GridCoord goal, bool diagonalMovement, HeuristicType heuristic)
{
	if (grid.DetectCollision(source) || grid.DetectCollision(goal))
		return {};

	if (grid.HasRegions() && grid.GetRegion(source) != grid.GetRegion(goal))
		return {};

	switch (heuristic)
	{
	case HeuristicType::Manhattan:
		return FindPathImpl<&Heuristic::Manhattan>(grid, source, goal, diagonalMovement);
	case HeuristicType::Euclidean:
		return FindPathImpl<&Heuristic::Euclidean>(grid, source, goal, diagonalMovement);
	case HeuristicType::Octagonal:
	default:
		return FindPathImpl<&Heuristic::Octagonal>(grid, source, goal, diagonalMovement);
	}
}

std::vector<Vector2f> FindPath(const AstarGrid& grid, Vector2f source, Vector2f goal, bool diagonalMovement, HeuristicType heuristic)
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
