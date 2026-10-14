-- Changing a body's Type at runtime makes physics create a new Box2D body without destroying
-- the old one. After the entity is destroyed the old body is left in the world holding its dead handle,
-- and raycasts or point queries that hit it build an Entity from that handle.

local frame = 0

local function CheckHits(what, entities)
	for _, entity in ipairs(entities) do
		Test.Expect(entity:IsSceneValid(), what .. " returns only live entities")
	end
end

function OnUpdate(deltaTime)
	frame = frame + 1
	local shifter = CurrentScene:FindEntity("Shifter")
	if frame == 2 then
		Log.Info("ChangeBodyType: changing Shifter from Static to Kinematic")
		shifter:GetRigidBody2DComponent().Type = BodyType.Kinematic
	elseif frame == 6 then
		Log.Info("ChangeBodyType: destroying Shifter")
		shifter:Destroy()
	elseif frame == 10 then
		Log.Info("ChangeBodyType: raycasting and querying where Shifter was")
		local hit = CurrentScene:RayCast2D(Vec2.new(-5, 0), Vec2.new(5, 0))
		CheckHits("RayCast2D", hit.Hit and { hit.Entity } or {})
		local hits = CurrentScene:MultiRayCast2D(Vec2.new(-5, 0), Vec2.new(5, 0))
		local multi = {}
		for _, h in ipairs(hits) do table.insert(multi, h.Entity) end
		CheckHits("MultiRayCast2D", multi)
		CheckHits("QueryPoint", CurrentScene:QueryPoint(Vec2.new(0, 0)))
		Test.Pass("ChangeBodyType: checked the queries")
	end
end
