#include "Pathfinding.h"

#include "Scene/Entity.h"
#include "Scene/Components/TilemapComponent.h"
#include "Logging/Logger.h"

namespace Pathfinding
{
namespace
{
	// 2D affine map between world space and the tilemap's grid space (x = column, y = row, 1 unit per tile)
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

		// Tile rows run down local -y
		Vector2f WorldToGrid(Vector2f world) const
		{
			Vector2f d = world - origin;
			float localX = (d.x * axisY.y - axisY.x * d.y) / determinant;
			float localY = (axisX.x * d.y - d.x * axisX.y) / determinant;
			return Vector2f(localX, -localY);
		}

		Vector2f GridToWorld(Vector2f grid) const
		{
			return origin + axisX * grid.x + axisY * -grid.y;
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

	if (tilemap->orientation != TilemapComponent::Orientation::orthogonal)
	{
		CLIENT_ERROR("Pathfinding.FindPath: only orthogonal tilemaps are supported");
		return path;
	}

	TilemapSpace space(tilemapEntity.GetComponent<TransformComponent>());
	if (!space.IsValid())
		return path;

	path = Astar::FindPath(tilemap->GetPathfindingGrid(), space.WorldToGrid(start), space.WorldToGrid(goal), diagonalMovement);

	for (Vector2f& point : path)
		point = space.GridToWorld(point);

	return path;
}
}
