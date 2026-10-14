-- Pathfinding on a hexagonal tilemap: reachable goals on both sides of the wall, a wall tile and a point off the map.
require("Scripts/Pathfinding/PathfindingTestCommon")

local goals = {
	{ name = "bottom right", column = 14, row = 8, reachable = true },
	{ name = "top right, past the wall", column = 13, row = 1, reachable = true },
	{ name = "wall tile", column = 10, row = 2, reachable = false },
	{ name = "off the map", x = 1000, y = 1000, reachable = false },
	{ name = "back to the start", column = 1, row = 1, reachable = true },
}

local test

function OnCreate()
	test = CreatePathfindingTest(CurrentScene:FindEntity("Level"), CurrentEntity, goals)
end

function OnUpdate(deltaTime)
	test:Update()
end
