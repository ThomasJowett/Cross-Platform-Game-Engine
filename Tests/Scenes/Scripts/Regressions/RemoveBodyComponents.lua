-- Removing a body's RigidBody2D and collider at runtime leaves its Box2D body in the world. Once
-- the entity is destroyed, raycasts and point queries that hit it build an Entity from a dead handle.

local frame = 0

local function CheckHits(what, entities)
	for _, entity in ipairs(entities) do
		Test.Expect(entity:IsSceneValid(), what .. " returns only live entities")
	end
end

function OnUpdate(deltaTime)
	frame = frame + 1
	local block = CurrentScene:FindEntity("Block")
	if frame == 2 then
		Log.Info("RemoveBodyComponents: removing Block's RigidBody2D and BoxCollider2D")
		block:RemoveRigidBody2DComponent()
		block:RemoveBoxCollider2DComponent()
	elseif frame == 6 then
		Log.Info("RemoveBodyComponents: destroying Block")
		block:Destroy()
	elseif frame == 10 then
		Log.Info("RemoveBodyComponents: raycasting and querying where Block was")
		local hit = CurrentScene:RayCast2D(Vec2.new(-5, 0), Vec2.new(5, 0))
		CheckHits("RayCast2D", hit.Hit and { hit.Entity } or {})
		CheckHits("QueryPoint", CurrentScene:QueryPoint(Vec2.new(0, 0)))
		Test.Pass("RemoveBodyComponents: checked the queries")
	end
end
