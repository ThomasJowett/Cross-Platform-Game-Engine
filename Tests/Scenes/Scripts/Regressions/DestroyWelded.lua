-- PhysicsEngine2D::DestroyEntity calls DestroyBody, which already destroys the bodies' joints,
-- then DestroyJoint on the same weld joint. Physics unparents bodies at start, so find the child by name.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 5 then
		Log.Info("DestroyWelded: destroying the welded child")
		CurrentScene:FindEntity("Welded Child"):Destroy()
	elseif frame == 10 then
		Log.Info("DestroyWelded: destroying the weld parent")
		CurrentScene:FindEntity("Weld Parent"):Destroy()
	elseif frame == 20 then
		Test.Pass("DestroyWelded: survived destroying welded entities")
	end
end
