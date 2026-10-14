-- Pathfinding on an orthogonal tilemap: reachable goals, a sealed room, a wall and a point off the map.
require("Scripts/Pathfinding/PathfindingTestCommon")

local goals = {
	{ name = "far corner", column = 22, row = 12, reachable = true },
	{ name = "sealed room", column = 17, row = 3, reachable = false },
	{ name = "wall tile", column = 7, row = 1, reachable = false },
	{ name = "off the map", x = 1000, y = 1000, reachable = false },
	{ name = "middle of the map", column = 9, row = 2, reachable = true },
	{ name = "back to the start", column = 1, row = 1, reachable = true },
}

local test

function OnCreate()
	test = CreatePathfindingTest(CurrentScene:FindEntity("Level"), CurrentEntity, goals)
end

function OnUpdate(deltaTime)
	test:Update()
end
