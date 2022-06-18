#pragma once

#include <vector>
#include <functional>
#include <set>
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

		GridCoord operator + (const GridCoord& other) const
		{
			return { x + other.x, y + other.y };
		}

		GridCoord operator - (const GridCoord& other) const
		{
			return { x - other.x, y - other.y };
		}
	};

	struct AstarGrid
	{
		int width;
		int height;
		float cellWidth;
		float cellHeight;

		Vector2f origin;

		bool** collisions;

		AstarGrid(int width, int height, float cellWidth, float cellHeight, Vector2f origin, bool** collisions)
			:width(width), height(height), cellWidth(cellWidth), cellHeight(cellHeight), origin(origin), collisions(collisions) {}

		bool PositionToGridCoord(Vector2f position, GridCoord& coordinate) const
		{
			Vector2f vPosition = position - origin;

			vPosition.y = vPosition.x / (float)width;
			vPosition.x = vPosition.y / (float)height;

			coordinate.x = (uint32_t)floor(position.x);
			coordinate.y = (uint32_t)floor(position.y);

			return (coordinate.x < width&& coordinate.y < height);
		}

		bool GridCoordToPosition(GridCoord coordinate, Vector2f& position) const
		{
			position.x = ((float)(coordinate.x * width) + (cellWidth / 2.0f));
			position.y = ((float)(coordinate.y * height) + (cellHeight / 2.0f));

			position += origin;

			return(coordinate.x < width&& coordinate.y < height);
		}

		bool DetectCollision(GridCoord coordinate) const
		{
			if (coordinate.x > width || coordinate.y > height)
			{
				return false;
			}

			return collisions[coordinate.x][coordinate.y];
		}
	};

	using HeuristicFunction = std::function<uint32_t(GridCoord, GridCoord)>;

	struct Node
	{
		uint32_t G;
		uint32_t H;
		GridCoord coordinates;
		Node* parent;

		Node(GridCoord coordinates, Node* parent = nullptr);

		uint32_t GetScore();
	};

	using NodeSet = std::set<Node*>;

	class Generator
	{
		Generator();
		Node* FindNodeOnList(const NodeSet& nodes, GridCoord coordinates);
		void ReleaseNodes(NodeSet& nodes);

	public:
		static Generator* GetInstance();

		void SetDiagonalMovement(bool enable);
		void SetHeuristic(HeuristicFunction function);
		std::vector<Vector2f> FindPath(Vector2f source, Vector2f goal, AstarGrid* grid);

	private:
		HeuristicFunction m_Heuristic;
		std::vector<GridCoord> m_Direction;
		std::vector<GridCoord> m_Collisions;
		uint32_t m_Directions;
	};

	class Heuristic
	{
		static GridCoord GetDelta(GridCoord source, GridCoord goal);

	public:
		static uint32_t Manhattan(GridCoord source, GridCoord goal);
		static uint32_t Euclidean(GridCoord source, GridCoord goal);
		static uint32_t Octagonal(GridCoord source, GridCoord goal);
	};
}