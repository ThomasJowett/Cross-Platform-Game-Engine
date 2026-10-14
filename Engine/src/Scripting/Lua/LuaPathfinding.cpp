#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"
#include "AI/Pathfinding.h"
#include "Scene/Entity.h"

namespace Lua
{
void BindPathfinding(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table pathfinding = state.create_table("Pathfinding");
	LuaManager::AddIdentifier("Pathfinding", "A* pathfinding");

	SetFunction(pathfinding, "Pathfinding", "FindPath",
		"FindPath(start, goal, tilemapEntity, [allowDiagonal = true]) - array of Vector2f tile centres from start to goal around an orthogonal, isometric or hexagonal tilemap's colliding tiles (allowDiagonal is ignored on hex maps), empty if there is no path",
		[](Vector2f start, Vector2f goal, Entity tilemapEntity, sol::optional<bool> allowDiagonal)
		{
			return sol::as_table(Pathfinding::FindPath(start, goal, tilemapEntity, allowDiagonal.value_or(true)));
		});

	SetFunction(pathfinding, "Pathfinding", "GetTileCentre",
		"GetTileCentre(tilemapEntity, column, row) - the world position of the centre of a tile of an orthogonal, isometric or hexagonal tilemap; columns and rows start at 0 in the top left",
		[](Entity tilemapEntity, int column, int row)
		{
			return Pathfinding::GetTileCentre(tilemapEntity, column, row);
		});
}
}
