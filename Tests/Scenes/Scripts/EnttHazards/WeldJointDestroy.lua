-- Hazard: destroying a welded entity. PhysicsEngine2D::DestroyEntity calls DestroyBody (which already
-- destroys attached joints) and then DestroyJoint on the same joint. Reaching the end means no crash.
-- Physics unparents bodies at runtime start, so the child is found by name rather than by path.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 5 then
		Log.Info("WeldJointDestroy: destroying the welded child")
		CurrentScene:FindEntity("Welded Child"):Destroy()
	elseif frame == 10 then
		Log.Info("WeldJointDestroy: destroying the weld parent")
		CurrentScene:FindEntity("Weld Parent"):Destroy()
	elseif frame == 20 then
		Test.Pass("WeldJointDestroy: survived destroying a welded entity")
	end
end
