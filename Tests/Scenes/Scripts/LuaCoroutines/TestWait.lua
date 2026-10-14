-- Wait(seconds) resumes on the first frame at or after that much game time, and not before.
require("Scripts/LuaCoroutines/CoroutineCommon")

local elapsed = 0
local lastDelta = 0
local resumedAt = nil
local resumedFrameDelta = nil

function OnCreate()
	StartCoroutine(function()
		Wait(0.5)
		resumedAt = elapsed
		resumedFrameDelta = lastDelta
	end)
end

function OnUpdate(deltaTime)
	elapsed = elapsed + deltaTime
	lastDelta = deltaTime
	if elapsed > 2.0 and resumedAt == nil then
		Report("Wait", false, "never resumed")
	elseif resumedAt ~= nil then
		-- Not before 0.5s, and the frame before it resumed was still under 0.5s
		local previous = resumedAt - resumedFrameDelta
		Report("Wait", resumedAt >= 0.5 and previous < 0.5, "resumed at " .. tostring(resumedAt) .. "s (previous frame " .. tostring(previous) .. "s), expected the first frame at or after 0.5s")
	end
end
