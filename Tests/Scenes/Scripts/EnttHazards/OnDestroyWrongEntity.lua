-- Hazard: Lua OnDestroy runs from ~LuaScriptComponent. EnTT's swap-and-pop copies the last
-- component into the removed slot, then destroys the leftover last copy, firing OnDestroy for
-- an entity that is still alive. Destroys "Victim" and checks only Victim got OnDestroy.
-- The controller counts its own OnDestroy too, since it may be the last script in the pool.

local frame = 0
local names = { "Victim", "Bystander 1", "Bystander 2", "Controller" }
local reported = false

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 1 then
		Log.Info("OnDestroyWrongEntity: destroying Victim")
		CurrentScene:FindEntity("Victim"):Destroy()
	elseif frame == 5 then
		local calls = _G.DestroyCalls or {}
		for _, name in ipairs(names) do
			local expected = (name == "Victim") and 1 or 0
			Test.Equal(calls[name] or 0, expected, name .. " OnDestroy calls")
		end
		for _, name in ipairs({ "Bystander 1", "Bystander 2" }) do
			local updates = (_G.UpdateCounts or {})[name] or 0
			Test.Expect(CurrentScene:FindEntity(name):IsSceneValid() and updates >= 4, name .. " is still alive and updating (" .. updates .. " updates)")
		end
		reported = true
		Test.Pass("OnDestroyWrongEntity: checked OnDestroy calls")
	end
end

function OnDestroy()
	if reported then return end
	_G.DestroyCalls = _G.DestroyCalls or {}
	_G.DestroyCalls["Controller"] = (_G.DestroyCalls["Controller"] or 0) + 1
	Log.Info("OnDestroyWrongEntity: OnDestroy called for Controller")
end
