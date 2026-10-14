-- Checks the removed script's coroutine stopped.
require("Scripts/LuaCoroutines/CoroutineCommon")

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 10 then
		local atRemove = _G.RemoveScriptTicksAtRemove
		local final = _G.RemoveScriptTicks
		Report("RemoveScript", atRemove ~= nil and final ~= nil and final - atRemove <= 1,
			"ticks at remove=" .. tostring(atRemove) .. ", final=" .. tostring(final))
	end
end
