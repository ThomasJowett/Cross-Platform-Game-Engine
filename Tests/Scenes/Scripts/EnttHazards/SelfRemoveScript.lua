-- Hazard: a script removing its own LuaScriptComponent from inside OnUpdate. The component (and the
-- sol::protected_function currently executing) is destroyed mid-call; C++ then keeps using it.

local removed = false

function OnUpdate(deltaTime)
	if removed then
		Test.Fail("SelfRemoveScript: OnUpdate ran again after removing its own script")
		return
	end
	removed = true
	Log.Info("SelfRemoveScript: removing own LuaScriptComponent")
	CurrentEntity:RemoveLuaScriptComponent()
	Log.Info("SelfRemoveScript: still executing Lua after removal")
	_G.SelfRemoved = true
end

function OnDestroy()
	Log.Info("SelfRemoveScript: OnDestroy called for the self-removing script")
end
