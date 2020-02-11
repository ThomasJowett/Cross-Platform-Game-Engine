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
		return static_cast <uint32_t>(10 * (delta.x + delta.y) + (-6) * min(delta.x, delta.y));
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

	Node * Generator::FindNodeOnList(NodeSet & nodes, GridCoord coordinates)
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

	void Generator::ReleaseNodes(NodeSet & nodes)
	{
		for (auto it = nodes.begin(); it != nodes.end();)
		{
			delete *it;
			it = nodes.erase(it);

		}
	}

	Generator * Generator::GetInstance()
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

	std::vector<Vector2f> Generator::FindPath(Vector2f source, Vector2f goal)
	{
		//TODO: find path
		return std::vector<Vector2f>();
	}

	Node::Node(GridCoord coordinates, Node * parent)
		:m_Parent(parent), m_Coordinates(coordinates)
	{
		G = H = 0;
	}

	uint32_t Node::GetScore()
	{
		return G + H;
	}
}