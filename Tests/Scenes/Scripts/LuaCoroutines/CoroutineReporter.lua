-- Collects the results from the coroutine test entities (via _G.CoroutineTests) and reports them.

local elapsed = 0
local reported = false

function OnCreate()
	-- TestError's deliberate coroutine error
	Test.ExpectErrors(1)
end

function OnUpdate(deltaTime)
	elapsed = elapsed + deltaTime
	if reported or elapsed < 3 then return end
	reported = true

	local results = _G.CoroutineTests or {}
	local expected = { "Sequence", "Wait", "Stop", "Error", "WaitFixed", "WaitOutside", "DestroyEntity", "RemoveScript", "Arguments" }
	for _, name in ipairs(expected) do
		local result = results[name]
		Test.Expect(result == true, name .. (result == true and "" or (" - " .. tostring(result or "never reported"))))
	end
	Test.Pass("LuaCoroutines: checked " .. #expected .. " coroutine tests")
end
