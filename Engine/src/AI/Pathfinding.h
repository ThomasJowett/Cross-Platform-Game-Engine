#pragma once

#include "Astar.h"
#include "math/Vector2f.h"

#include <vector>

class Entity;

namespace Pathfinding
{
	// World-space tile centres from start to goal across an orthogonal, isometric or hexagonal tilemap; tiles with a collision shape are blocked
	std::vector<Vector2f> FindPath(Vector2f start, Vector2f goal, Entity tilemapEntity, bool diagonalMovement = true);

	// World-space centre of the tile at the given column and row, or the origin if the entity has no usable tilemap
	Vector2f GetTileCentre(Entity tilemapEntity, int column, int row);
}
