-- Bug: SceneGraph::Unparent keeps a pointer to the child's HierarchyComponent across removing the parent's
-- component, which can move the child's component in memory; the unparent then writes to the wrong place and
-- the child keeps a link to its old parent. Physics unparents a body when it first initialises it.
-- The hierarchy is built so Middle's component is last in the pool, Root's only child is Middle, and Middle
-- has a child of its own (so its component is kept).

local frame = 0
local root, middle, leaf, other, a, z

local function New(name)
	local entity = CurrentScene:CreateEntity(name)
	entity:AddTransformComponent()
	return entity
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 1 then
		root, middle, leaf, other, a, z = New("Root"), New("Middle"), New("Leaf"), New("Other"), New("A"), New("Z")
		root:AddChild(a)        -- Root gets its component first
		leaf:AddChild(other)    -- Leaf gets a component and a child
		middle:AddChild(leaf)   -- Middle's component is created now
		root:AddChild(middle)   -- Root's children: A, Middle
		leaf:AddChild(z)        -- one more component after Middle's
		a:Destroy()             -- removing A's component moves Z into its slot, leaving Middle last
	elseif frame == 3 then
		Log.Info("UnparentStalePointer: giving Middle a body, so physics unparents it")
		middle:AddRigidBody2DComponent().GravityScale = 0
		middle:AddBoxCollider2DComponent()
	elseif frame == 8 then
		local parent = middle:GetParent()
		Test.Expect(not parent:IsSceneValid(), "Middle has no parent after physics unparented it"
			.. (parent:IsSceneValid() and (", but GetParent() returns " .. parent:GetName()) or ""))
		Log.Info("UnparentStalePointer: destroying Root")
		root:Destroy()
	elseif frame == 10 then
		Log.Info("UnparentStalePointer: destroying Middle")
		middle:Destroy()
	elseif frame == 12 then
		Test.Pass("UnparentStalePointer: destroyed the hierarchy")
	end
end
