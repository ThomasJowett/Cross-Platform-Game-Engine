-- Shared driver for the pathfinding scenes. CreatePathfindingTest(level, agent, goals) returns an object whose
-- Update() is called every frame. For each goal, in order, it asks Pathfinding.FindPath for a route from the agent's
-- current position, checks a route exists exactly when the goal says so, then drives the agent along it and checks
-- it arrives. A goal is { name, column, row, reachable } for a tile, or { name, x, y, reachable } for a world position.
-- The agent needs a dynamic RigidBody2D with gravity scale 0 and a CircleCollider2D.

local Speed = 5.0
local WaypointRadius = 0.1
local SlowdownRadius = 0.5
local ArrivalRadius = 0.3
local MaxFramesPerGoal = 3000

function CreatePathfindingTest(level, agent, goals)
	local test = { index = 0, path = {}, nextPoint = 1, frames = 0, goal = nil }

	local function Position()
		local position = agent:GetTransformComponent().Position
		return position.x, position.y
	end

	local function GoalPosition(goal)
		if goal.column then
			local centre = Pathfinding.GetTileCentre(level, goal.column, goal.row)
			return centre.x, centre.y
		end
		return goal.x, goal.y
	end

	local function Stop()
		agent:GetRigidBody2DComponent():SetLinearVelocity(Vec2.new(0, 0))
	end

	local function NextGoal()
		test.index = test.index + 1
		test.goal = goals[test.index]
		test.path = {}
		test.frames = 0

		if test.goal == nil then
			Test.Pass("Pathfinding: all " .. #goals .. " goals behaved as expected")
			return
		end

		local goal = test.goal
		local goalX, goalY = GoalPosition(goal)
		local x, y = Position()
		test.path = Pathfinding.FindPath(Vec2.new(x, y), Vec2.new(goalX, goalY), level)

		Test.Equal(#test.path > 0, goal.reachable, goal.name .. (goal.reachable and ": a path exists" or ": there is no path"))
		if #test.path == 0 or not goal.reachable then
			test.path = {}
			NextGoal()
			return
		end

		-- The first waypoint is the tile the agent is already on
		test.nextPoint = math.min(2, #test.path)
	end

	function test:Update()
		if self.goal == nil and self.index == 0 then
			NextGoal()
		end

		local target = self.path[self.nextPoint]
		if target == nil then
			return
		end

		self.frames = self.frames + 1
		if self.frames > MaxFramesPerGoal then
			Test.Fail(self.goal.name .. ": the agent did not reach the goal within " .. MaxFramesPerGoal .. " frames")
			self.path = {}
			return
		end

		local x, y = Position()
		local dx, dy = target.x - x, target.y - y
		local distance = math.sqrt(dx * dx + dy * dy)
		local isLast = self.nextPoint == #self.path

		if distance <= WaypointRadius then
			self.nextPoint = self.nextPoint + 1
			if isLast then
				Stop()
				local goalX, goalY = GoalPosition(self.goal)
				local remaining = math.sqrt((goalX - x) ^ 2 + (goalY - y) ^ 2)
				Test.Expect(remaining <= ArrivalRadius, self.goal.name .. ": the agent arrived at the goal (" .. string.format("%.2f", remaining) .. " away)")
				NextGoal()
			end
			return
		end

		local speed = Speed
		if isLast then
			speed = Speed * math.min(1.0, distance / SlowdownRadius)
		end
		agent:GetRigidBody2DComponent():SetLinearVelocity(Vec2.new(dx / distance * speed, dy / distance * speed))
	end

	return test
end
