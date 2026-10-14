-- Removing the script stops its coroutines.
require("Scripts/LuaCoroutines/CoroutineCommon")

local frame = 0

function OnCreate()
	StartCoroutine(function()
		while true do
			_G.RemoveScriptTicks = (_G.RemoveScriptTicks or 0) + 1
			coroutine.yield()
		end
	end)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 3 then
		_G.RemoveScriptTicksAtRemove = _G.RemoveScriptTicks
		CurrentEntity:RemoveLuaScriptComponent()
	end
end
