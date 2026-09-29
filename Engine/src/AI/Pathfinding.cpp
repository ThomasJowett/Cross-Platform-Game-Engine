#include "Pathfinding.h"

#include "Scene/Entity.h"
#include "Scene/Components/TilemapComponent.h"
#include "Logging/Logger.h"

namespace Pathfinding
{
namespace
{
	// 2D affine map between world space and the tilemap's local space
	struct TilemapSpace
	{
		Vector2f origin;
		Vector2f axisX;
		Vector2f axisY;
		float determinant;

		explicit TilemapSpace(const TransformComponent& transform)
		{
			Matrix4x4 matrix = transform.GetParentMatrix() * transform.GetLocalMatrix();
			Vector3f o = matrix * Vector3f(0.0f, 0.0f, 0.0f);
			origin = Vector2f(o.x, o.y);
			axisX = Vector2f(matrix * Vector3f(1.0f, 0.0f, 0.0f)) - origin;
			axisY = Vector2f(matrix * Vector3f(0.0f, 1.0f, 0.0f)) - origin;
			determinant = axisX.x * axisY.y - axisY.x * axisX.y;
		}

		bool IsValid() const { return std::abs(determinant) > 1e-6f; }

		Vector2f WorldToLocal(Vector2f world) const
		{
			Vector2f d = world - origin;
			return Vector2f((d.x * axisY.y - axisY.x * d.y) / determinant, (axisX.x * d.y - d.x * axisX.y) / determinant);
		}

		Vector2f LocalToWorld(Vector2f local) const
		{
			return origin + axisX * local.x + axisY * local.y;
		}
	};
}

std::vector<Vector2f> FindPath(Vector2f start, Vector2f goal, Entity tilemapEntity, bool diagonalMovement)
{
	std::vector<Vector2f> path;

	if (!tilemapEntity)
	{
		CLIENT_ERROR("Pathfinding.FindPath: invalid tilemap entity");
		return path;
	}

	TilemapComponent* tilemap = tilemapEntity.TryGetComponent<TilemapComponent>();
	if (!tilemap)
	{
		CLIENT_ERROR("Pathfinding.FindPath: entity has no TilemapComponent");
		return path;
	}

	if (!tilemap->SupportsPathfinding())
	{
		CLIENT_ERROR("Pathfinding.FindPath: staggered tilemaps are not supported");
		return path;
	}

	TilemapSpace space(tilemapEntity.GetComponent<TransformComponent>());
	if (!space.IsValid())
		return path;

	Astar::GridCoord startCell, goalCell;
	if (!tilemap->LocalToCell(space.WorldToLocal(start), startCell) || !tilemap->LocalToCell(space.WorldToLocal(goal), goalCell))
		return path;

	std::vector<Astar::GridCoord> cells = Astar::FindPath(tilemap->GetPathfindingGrid(), startCell, goalCell, diagonalMovement);
	path.reserve(cells.size());
	for (const Astar::GridCoord& cell : cells)
		path.push_back(space.LocalToWorld(tilemap->CellToLocal(cell)));

	return path;
}
}
