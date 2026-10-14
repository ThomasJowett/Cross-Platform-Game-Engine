-- StartCoroutine runs straight away up to the first yield, then resumes once per frame.
require("Scripts/LuaCoroutines/CoroutineCommon")

local steps = {}
local frame = 0

function OnCreate()
	StartCoroutine(function()
		table.insert(steps, "start:" .. frame)
		coroutine.yield()
		table.insert(steps, "after yield 1:" .. frame)
		coroutine.yield()
		table.insert(steps, "after yield 2:" .. frame)
	end)
	table.insert(steps, "StartCoroutine returned:" .. frame)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 5 then
		local got = table.concat(steps, ", ")
		-- Ran immediately in OnCreate (frame 0), then once in each of the next two frames
		local expected = "start:0, StartCoroutine returned:0, after yield 1:1, after yield 2:2"
		Report("Sequence", got == expected, "got '" .. got .. "', expected '" .. expected .. "'")
	end
end
