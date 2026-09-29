#include "Astar.h"

#include <queue>
#include <limits>

namespace Astar
{
namespace
{
	constexpr uint32_t c_StraightCost = 10;
	constexpr uint32_t c_DiagonalCost = 14;

	// Exact on an open grid for 4-way movement
	uint32_t Manhattan(GridCoord source, GridCoord goal)
	{
		uint32_t dx = (uint32_t)std::abs(source.x - goal.x);
		uint32_t dy = (uint32_t)std::abs(source.y - goal.y);
		return c_StraightCost * (dx + dy);
	}

	// Exact on an open grid for 8-way movement
	uint32_t Octile(GridCoord source, GridCoord goal)
	{
		uint32_t dx = (uint32_t)std::abs(source.x - goal.x);
		uint32_t dy = (uint32_t)std::abs(source.y - goal.y);
		return c_StraightCost * std::max(dx, dy) + (c_DiagonalCost - c_StraightCost) * std::min(dx, dy);
	}

	// Exact on an open hex grid; offset coordinates converted to axial
	uint32_t HexDistance(GridCoord source, GridCoord goal)
	{
		int sourceR = source.y - (source.x - (source.x & 1)) / 2;
		int goalR = goal.y - (goal.x - (goal.x & 1)) / 2;
		int dq = source.x - goal.x;
		int dr = sourceR - goalR;
		return c_StraightCost * (uint32_t)((std::abs(dq) + std::abs(dr) + std::abs(dq + dr)) / 2);
	}

	// First 4 are orthogonal, last 4 diagonal
	const GridCoord c_Directions[8] = {
		{ 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
		{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
	};

	// Hex neighbours depend on column parity
	const GridCoord c_HexEvenColumnDirections[6] = {
		{ 1, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }
	};
	const GridCoord c_HexOddColumnDirections[6] = {
		{ 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { 0, -1 }
	};

	enum class Movement
	{
		Square4,
		Square8,
		Hex
	};

	template<Movement M>
	constexpr int DirectionCount() { return M == Movement::Square8 ? 8 : (M == Movement::Hex ? 6 : 4); }

	template<Movement M>
	const GridCoord* Directions(GridCoord cell)
	{
		if constexpr (M == Movement::Hex)
			return (cell.x & 1) ? c_HexOddColumnDirections : c_HexEvenColumnDirections;
		else
			return c_Directions;
	}

	template<Movement M>
	uint32_t Heuristic(GridCoord source, GridCoord goal)
	{
		if constexpr (M == Movement::Hex)
			return HexDistance(source, goal);
		else if constexpr (M == Movement::Square8)
			return Octile(source, goal);
		else
			return Manhattan(source, goal);
	}

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

template<Movement M>
static void FloodFillRegions(const AstarGrid& grid, std::vector<uint32_t>& regions)
{
	const size_t cellCount = grid.collisions.size();
	regions.assign(cellCount, 0);

	std::vector<size_t> queue;
	queue.reserve(cellCount);
	uint32_t nextRegion = 1;

	for (size_t seed = 0; seed < cellCount; seed++)
	{
		if (grid.collisions[seed] || regions[seed])
			continue;

		queue.clear();
		queue.push_back(seed);
		regions[seed] = nextRegion;

		for (size_t head = 0; head < queue.size(); head++)
		{
			GridCoord current((int)(queue[head] % grid.width), (int)(queue[head] / grid.width));
			const GridCoord* directions = Directions<M>(current);
			for (int i = 0; i < DirectionCount<M>(); i++)
			{
				GridCoord neighbour = current + directions[i];
				if (grid.DetectCollision(neighbour))
					continue;

				size_t index = grid.Index(neighbour);
				if (!regions[index])
				{
					regions[index] = nextRegion;
					queue.push_back(index);
				}
			}
		}
		nextRegion++;
	}
}

void AstarGrid::LabelRegions()
{
	if (topology == Topology::Hex)
		FloodFillRegions<Movement::Hex>(*this, m_Regions);
	else
		FloodFillRegions<Movement::Square4>(*this, m_Regions);
}

template<Movement M>
static std::vector<GridCoord> FindPathImpl(const AstarGrid& grid, GridCoord source, GridCoord goal)
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
	uint32_t sourceH = Heuristic<M>(source, goal);
	open.push({ sourceH, sourceH, sourceIndex });

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

		const GridCoord* directions = Directions<M>(current);
		for (int i = 0; i < DirectionCount<M>(); i++)
		{
			const GridCoord& direction = directions[i];
			GridCoord neighbour = current + direction;

			if (grid.DetectCollision(neighbour))
				continue;

			bool isDiagonal = M == Movement::Square8 && i >= 4;

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
				uint32_t h = Heuristic<M>(neighbour, goal);
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

std::vector<GridCoord> FindPath(const AstarGrid& grid, GridCoord source, GridCoord goal, bool diagonalMovement)
{
	if (grid.DetectCollision(source) || grid.DetectCollision(goal))
		return {};

	if (grid.HasRegions() && grid.GetRegion(source) != grid.GetRegion(goal))
		return {};

	if (grid.topology == Topology::Hex)
		return FindPathImpl<Movement::Hex>(grid, source, goal);
	return diagonalMovement ? FindPathImpl<Movement::Square8>(grid, source, goal) : FindPathImpl<Movement::Square4>(grid, source, goal);
}

std::vector<Vector2f> FindPath(const AstarGrid& grid, Vector2f source, Vector2f goal, bool diagonalMovement)
{
	std::vector<Vector2f> path;
	if (grid.topology != Topology::Square)
		return path;

	GridCoord sourceCoords;
	GridCoord goalCoords;

	if (!grid.PositionToGridCoord(source, sourceCoords) || !grid.PositionToGridCoord(goal, goalCoords))
		return path;

	std::vector<GridCoord> cells = FindPath(grid, sourceCoords, goalCoords, diagonalMovement);
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
