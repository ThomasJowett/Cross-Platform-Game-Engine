-- At runtime start and when instantiating, rigid bodies leave the scene graph and keep their place in the world,
-- while everything else stays parented. Physics is set up before the first render, so this also checks that the
-- world position comes from the hierarchy rather than the cached world matrix.

local Epsilon = 0.01
local frame = 0

local function Find(name)
	return CurrentScene:FindEntity(name)
end

local function ParentName(name)
	local parent = Find(name):GetParent()
	return parent:IsSceneValid() and parent:GetName() or "(none)"
end

-- The names of the entities with a collider at a point
local function At(x, y)
	local names = {}
	for _, entity in ipairs(CurrentScene:QueryPoint(Vec2.new(x, y))) do
		table.insert(names, entity:GetName())
	end
	table.sort(names)
	return table.concat(names, ", ")
end

local function Near(actual, expected, message)
	Test.Expect(math.abs(actual - expected) <= Epsilon, message .. " (expected " .. expected .. ", got " .. actual .. ")")
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 3 then
		Test.Equal(ParentName("Child Body"), "(none)", "a rigid body child leaves the scene graph")
		Test.Equal(At(2, 0), "Child Body", "a rigid body child keeps its place in the world")
		Test.Equal(ParentName("Body Hat"), "Child Body", "a sprite under a rigid body stays parented to it")
		Test.Equal(At(6, 0), "Sibling Body", "a rigid body sibling has its body in the right place")
		Test.Equal(At(0, 0), "", "no body is left at the origin")
		Test.Equal(ParentName("Sibling Sprite"), "Group", "a sprite beside a rigid body stays parented")

		local flipped = Find("Flipped Body"):GetTransformComponent()
		Near(flipped.Position.x, 18, "a rigid body under a mirrored parent keeps its world position")
		Near(flipped.Scale.x, -1, "a rigid body under a mirrored parent keeps the flip")
		Test.Equal(At(18, 0), "Flipped Body", "a rigid body under a mirrored parent has its body in place")

		CurrentScene:InstantiateScene(LoadScene("Scenes/ParentedPhysicsPrefab.scene"), Vec3.new(0, -20, 0))
	elseif frame == 4 then
		Test.Equal(ParentName("Prefab Body"), "(none)", "an instantiated rigid body child leaves the scene graph")
		Test.Equal(At(2, -20), "Prefab Body", "an instantiated rigid body child keeps its place")
		Test.Equal(ParentName("Prefab Hat"), "Prefab Body", "an instantiated sprite stays under its rigid body")

		Test.Pass("ParentedPhysics: checked rigid bodies under parents")
	end
end
