-- Adding a RigidBody2D from OnFixedUpdate (or a contact callback) leaves it without a Box2D body
-- until the next physics step, but Scene::OnFixedUpdate's transform sync dereferences it straight away.

local fixedUpdates = 0
local spawned = nil
local startY = 5

function OnFixedUpdate()
	fixedUpdates = fixedUpdates + 1
	if fixedUpdates == 1 then
		Log.Info("AddBodyInFixedUpdate: spawning a dynamic body from OnFixedUpdate")
		spawned = CurrentScene:CreateEntity("Spawned Body")
		spawned:AddTransformComponent().Position = Vec3.new(0, startY, 0)
		spawned:AddRigidBody2DComponent().Type = BodyType.Dynamic
		spawned:AddBoxCollider2DComponent()  -- gives the body mass, so gravity moves it
	elseif fixedUpdates == 60 then
		local y = spawned:GetTransformComponent().Position.y
		Test.Expect(y < startY, "spawned body was simulated (y=" .. y .. ")")
		Test.Pass("AddBodyInFixedUpdate: checked the spawned body")
	end
end
