-- Checks the destroyed entity's coroutine stopped once its entity was gone.
require("Scripts/LuaCoroutines/CoroutineCommon")

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 10 then
		local atDestroy = _G.DestroyEntityTicksAtDestroy
		local final = _G.DestroyEntityTicks
		-- It may finish the frame it was destroyed in, but never runs after that
		Report("DestroyEntity", atDestroy ~= nil and final ~= nil and final - atDestroy <= 1,
			"ticks at destroy=" .. tostring(atDestroy) .. ", final=" .. tostring(final))
	end
end
