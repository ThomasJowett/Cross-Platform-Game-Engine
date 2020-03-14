#include "stdafx.h"
#include "Astar.h"


namespace Astar
{
	static Generator* instance = 0;

	GridCoord Heuristic::GetDelta(GridCoord source, GridCoord goal)
	{
		return GridCoord(abs(source.x - goal.x), abs(source.y - goal.y));
	}

	uint32_t Heuristic::Manhattan(GridCoord source, GridCoord goal)
	{
		auto delta = std::move(GetDelta(source, goal));
		return static_cast<uint32_t>(10 * (delta.x + delta.y));
	}

	uint32_t Heuristic::Euclidean(GridCoord source, GridCoord goal)
	{
		auto delta = std::move(GetDelta(source, goal));
		return static_cast<uint32_t>(10 * sqrt(pow(delta.x, 2) + pow(delta.y, 2)));
	}

	uint32_t Heuristic::Octagonal(GridCoord source, GridCoord goal)
	{
		auto delta = std::move(GetDelta(source, goal));
		return static_cast <uint32_t>(10 * (delta.x + delta.y) + (-6) * ((delta.x < delta.y) ? delta.x : delta.y));
	}

	Generator::Generator()
	{
		SetDiagonalMovement(true);
		SetHeuristic(&Heuristic::Octagonal);

		m_Direction = {
			{ 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
			{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
		};
	}

	Node* Generator::FindNodeOnList(NodeSet& nodes, GridCoord coordinates)
	{
		for (Node* node : nodes)
		{
			if (node->m_Coordinates == coordinates)
			{
				return node;
			}
		}
		return nullptr;
	}

	void Generator::ReleaseNodes(NodeSet& nodes)
	{
		for (auto it = nodes.begin(); it != nodes.end();)
		{
			delete* it;
			it = nodes.erase(it);

		}
	}

	Generator* Generator::GetInstance()
	{
		if (instance == 0)
		{
			instance = new Generator();
		}
		return instance;
	}

	void Generator::SetDiagonalMovement(bool enable)
	{
		m_Directions = (enable ? 8 : 4);
	}

	void Generator::SetHeuristic(HeuristicFunction function)
	{
		m_Heuristic = std::bind(function, std::placeholders::_1, std::placeholders::_2);
	}

	std::vector<Vector2f> Generator::FindPath(Vector2f source, Vector2f goal, AstarGrid* grid)
	{
		std::vector<Vector2f> path;

		GridCoord sourceCoords, goalCoords;

		if (!grid->PositionToGridCoord(source, sourceCoords))
			return path;

		if (!grid->PositionToGridCoord(goal, goalCoords))
			return path;

		Node* current = nullptr;
		NodeSet openSet, closedSet;

		openSet.insert(new Node(sourceCoords));

		while (!openSet.empty())
		{
			current = *openSet.begin();
			for (Node* node : openSet)
			{
				if (node->GetScore() <= current->GetScore())
				{
					current = node;
				}
			}

			if (current->m_Coordinates == goalCoords)
			{
				break; // found the goal so exit
			}

			closedSet.insert(current);
			openSet.erase(std::find(openSet.begin(), openSet.end(), current));

			for (uint32_t i = 0; i < m_Directions; i++)
			{
				GridCoord newCoords(current->m_Coordinates + m_Direction[i]);

				if (grid->DetectCollision(newCoords) || FindNodeOnList(closedSet, newCoords))
					continue;

				uint32_t totalcost = current->G + ((i < 4) ? 10 : 14);

				Node* successor = FindNodeOnList(openSet, newCoords);

				if (successor == nullptr)
				{
					successor = new Node(newCoords, current);
					successor->G = totalcost;
					successor->H = m_Heuristic(successor->m_Coordinates, goalCoords);
					openSet.insert(successor);
				}
				else if (totalcost < successor->G)
				{
					successor->m_Parent = current;
					successor->G = totalcost;
				}
			}
		}

		while (current != nullptr)
		{
			/*convert coordinates back into world positions*/
			Vector2f position;
			grid->GridCoordToPosition(current->m_Coordinates, position);
			path.push_back(position);
			current = current->m_Parent;
		}

		ReleaseNodes(openSet);
		ReleaseNodes(closedSet);
		return std::vector<Vector2f>();
	}

	Node::Node(GridCoord coordinates, Node* parent)
		:m_Parent(parent), m_Coordinates(coordinates)
	{
		G = H = 0;
	}

	uint32_t Node::GetScore()
	{
		return G + H;
	}
}