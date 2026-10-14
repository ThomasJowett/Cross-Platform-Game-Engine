-- A child's world position is its parent's world transform applied to its local position, through any number of
-- levels. Parent sits at (10, 0), turned a quarter turn and doubled in size, so a child's local (1, 0) lands at
-- (10, 2): the local offset is scaled to (2, 0), then rotated up to (0, 2). Entities are built in frame 1, and the
-- checks run in later frames so a full update has passed since each change.

local Epsilon = 0.01
local frame = 0
local parent, child, grandchild, other

local function Near(entity, x, y, message)
	local world = entity:GetTransformComponent():GetWorldPosition()
	Test.Expect(math.abs(world.x - x) <= Epsilon and math.abs(world.y - y) <= Epsilon,
		message .. " (expected " .. x .. ", " .. y .. " but got " .. string.format("%.2f, %.2f", world.x, world.y) .. ")")
end

local function New(name, x, y)
	local entity = CurrentScene:CreateEntity(name)
	entity:AddTransformComponent().Position = Vec3.new(x, y, 0)
	return entity
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 1 then
		parent = New("Parent", 10, 0)
		child = New("Child", 1, 0)
		grandchild = New("Grandchild", 1, 0)
		other = New("Other", -5, 0)
		parent:AddChild(child)
		child:AddChild(grandchild)

		local transform = parent:GetTransformComponent()
		transform.Rotation = Vec3.new(0, 0, math.pi / 2)
		transform.Scale = Vec3.new(2, 2, 1)
	elseif frame == 3 then
		Near(parent, 10, 0, "a parent with no parent sits at its own position")
		Near(child, 10, 2, "the child is scaled and rotated by its parent")
		Near(grandchild, 10, 4, "the grandchild is scaled and rotated by both ancestors")
		Test.Equal(child:GetParent():GetName(), "Parent", "the child's parent is Parent")
		Test.Equal(grandchild:GetParent():GetName(), "Child", "the grandchild's parent is Child")

		-- Moving the parent moves everything below it
		parent:GetTransformComponent().Position = Vec3.new(0, 5, 0)
	elseif frame == 5 then
		Near(child, 0, 7, "the child follows its parent when the parent moves")
		Near(grandchild, 0, 9, "the grandchild follows too")

		-- Turning the parent back removes the rotation but keeps the scale
		parent:GetTransformComponent().Rotation = Vec3.new(0, 0, 0)
	elseif frame == 7 then
		Near(child, 2, 5, "the child's offset stays scaled once the rotation is removed")

		-- Giving the grandchild a new parent re-bases it onto that parent's transform
		other:AddChild(grandchild)
	elseif frame == 9 then
		Test.Equal(grandchild:GetParent():GetName(), "Other", "the grandchild's parent is now Other")
		Near(grandchild, -4, 0, "the grandchild is positioned relative to its new parent")
		Near(child, 2, 5, "the old parent's child is unaffected by the move")
		Test.Pass("WorldTransforms: checked the hierarchy")
	end
end
