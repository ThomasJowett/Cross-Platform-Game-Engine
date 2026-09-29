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
		"FindPath(start, goal, tilemapEntity, [allowDiagonal = true]) - array of Vector2f tile centres from start to goal around the tilemap's colliding tiles, empty if there is no path",
		[](Vector2f start, Vector2f goal, Entity tilemapEntity, sol::optional<bool> allowDiagonal)
		{
			return sol::as_table(Pathfinding::FindPath(start, goal, tilemapEntity, allowDiagonal.value_or(true)));
		});
}
}
