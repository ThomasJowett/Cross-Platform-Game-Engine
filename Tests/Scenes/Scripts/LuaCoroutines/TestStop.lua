-- StopCoroutine stops a running coroutine for good.
require("Scripts/LuaCoroutines/CoroutineCommon")

local ticks = 0
local frame = 0
local handle = nil
local ticksWhenStopped = nil

function OnCreate()
	handle = StartCoroutine(function()
		while true do
			ticks = ticks + 1
			coroutine.yield()
		end
	end)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 3 then
		StopCoroutine(handle)
		ticksWhenStopped = ticks
	elseif frame == 8 then
		Report("Stop", ticks == ticksWhenStopped and ticks > 0, "ticks went from " .. tostring(ticksWhenStopped) .. " to " .. ticks .. " after StopCoroutine")
	end
end
