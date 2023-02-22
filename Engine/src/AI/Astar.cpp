#include "stdafx.h"
#include "Astar.h"

namespace Astar
{
	Generator* Generator::s_Instance = nullptr;

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

	Generator::Generator()
	{
		SetDiagonalMovement(true);
		SetHeuristic(&Heuristic::Octagonal);
	}

	Ref<Node> Generator::FindNodeOnList(const NodeSet& nodes, GridCoord coordinates) const
	{
		for (Ref<Node> node : nodes)
		{
			if (node->coordinates == coordinates)
			{
				return node;
			}
		}
		return nullptr;
	}

	void Generator::ReleaseNodes(NodeSet& nodes) const
	{
		for (auto it = nodes.begin(); it != nodes.end();)
		{
			it = nodes.erase(it);
		}
	}

	Generator* Generator::GetInstance()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = new Generator();
		}
		return s_Instance;
	}

	void Generator::SetDiagonalMovement(bool enable)
	{
		m_Directions = (enable ? 8 : 4);
	}

	void Generator::SetHeuristic(HeuristicFunction function)
	{
		m_Heuristic = std::bind(function, std::placeholders::_1, std::placeholders::_2);
	}

	std::vector<Vector2f> Generator::FindPath(Vector2f source, Vector2f goal, const AstarGrid* grid) const
	{
		std::vector<Vector2f> path;

		GridCoord sourceCoords;
		GridCoord goalCoords;

		if (!grid->PositionToGridCoord(source, sourceCoords))
			return path;

		if (!grid->PositionToGridCoord(goal, goalCoords))
			return path;

		Ref<Node> current = nullptr;
		NodeSet openSet;
		NodeSet closedSet;

		openSet.insert(CreateRef<Node>(sourceCoords));

		while (!openSet.empty())
		{
			current = *openSet.begin();
			for (Ref<Node> node : openSet)
			{
				if (node->GetScore() <= current->GetScore())
				{
					current = node;
				}
			}

			if (current->coordinates == goalCoords)
			{
				break; // found the goal so exit
			}

			closedSet.insert(current);
			openSet.erase(std::find(openSet.begin(), openSet.end(), current));

			for (uint32_t i = 0; i < m_Directions; i++)
			{
				GridCoord newCoords(current->coordinates + m_Direction[i]);

				if (grid->DetectCollision(newCoords) || FindNodeOnList(closedSet, newCoords))
					continue;

				uint32_t totalcost = current->G + ((i < 4) ? 10 : 14);

				Ref<Node> successor = FindNodeOnList(openSet, newCoords);

				if (successor == nullptr)
				{
					successor = CreateRef<Node>(newCoords, current);
					successor->G = totalcost;
					successor->H = m_Heuristic(successor->coordinates, goalCoords);
					openSet.insert(successor);
				}
				else if (totalcost < successor->G)
				{
					successor->parent = current;
					successor->G = totalcost;
				}
			}
		}

		while (current != nullptr)
		{
			/*convert coordinates back into world positions*/
			Vector2f position;
			grid->GridCoordToPosition(current->coordinates, position);
			path.push_back(position);
			current = current->parent;
		}

		ReleaseNodes(openSet);
		ReleaseNodes(closedSet);
		return std::vector<Vector2f>();
	}

	Node::Node(GridCoord coordinates, Ref<Node> parent)
		:coordinates(coordinates), parent(parent)
	{
		G = H = 0;
	}

	uint32_t Node::GetScore() const
	{
		return G + H;
	}
}