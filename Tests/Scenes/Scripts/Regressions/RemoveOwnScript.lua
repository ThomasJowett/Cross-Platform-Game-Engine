-- A script removing its own LuaScriptComponent frees the function that is currently running.

local removed = false

function OnUpdate(deltaTime)
	if removed then
		Test.Fail("RemoveOwnScript: OnUpdate ran again after removing its own script")
		return
	end
	removed = true
	Log.Info("RemoveOwnScript: removing own LuaScriptComponent")
	CurrentEntity:RemoveLuaScriptComponent()
	Log.Info("RemoveOwnScript: still running after the removal call")
	_G.RemoveOwnScriptFinished = true
end
