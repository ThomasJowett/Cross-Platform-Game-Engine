-- An error inside one coroutine stops only that coroutine; the script and its other coroutines carry on.
require("Scripts/LuaCoroutines/CoroutineCommon")

local frame = 0
local otherTicks = 0
local reachedAfterError = false

function OnCreate()
	StartCoroutine(function()
		coroutine.yield()
		error("deliberate coroutine error")
		reachedAfterError = true
	end)
	StartCoroutine(function()
		while true do
			otherTicks = otherTicks + 1
			coroutine.yield()
		end
	end)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 8 then
		Report("Error", not reachedAfterError and otherTicks >= 7, "reachedAfterError=" .. tostring(reachedAfterError) .. " otherTicks=" .. otherTicks)
	end
end
