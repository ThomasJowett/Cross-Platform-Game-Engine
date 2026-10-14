-- Counts OnUpdate calls per entity. "Counter 2" also removes the scripts from Counter 1 and
-- Counter 3 in the middle of the frame, while Scene::OnUpdate is iterating the script pool.

local myName = nil
local calls = 0

function OnCreate()
	myName = CurrentEntity:GetName()
end

function OnUpdate(deltaTime)
	calls = calls + 1
	_G.UpdateCounts = _G.UpdateCounts or {}
	_G.UpdateCounts[myName] = (_G.UpdateCounts[myName] or 0) + 1

	if myName == "Counter 2" and calls == 3 then
		Log.Info("ScriptRemovalDuringUpdate: Counter 2 removing scripts from Counter 1 and Counter 3")
		CurrentScene:FindEntity("Counter 1"):RemoveLuaScriptComponent()
		CurrentScene:FindEntity("Counter 3"):RemoveLuaScriptComponent()
		_G.ScriptsRemoved = true
	end
end

function OnDestroy()
	Log.Info("ScriptRemovalDuringUpdate: OnDestroy called for " .. tostring(myName))
end
