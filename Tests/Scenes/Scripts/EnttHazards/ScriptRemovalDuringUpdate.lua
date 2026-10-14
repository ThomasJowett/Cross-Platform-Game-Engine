-- Removing LuaScriptComponents while Scene::OnUpdate is iterating them must not make other scripts update
-- twice or skip an update, and removed scripts must stop.
-- Checks run in OnFixedUpdate: fixed updates happen between whole update passes, so at that point every
-- script still running must have had exactly as many OnUpdate calls as this controller.

local updates = 0
local checks = 0
local failures = 0
local frozen = nil
local reported = false

local function Count(name)
	return (_G.UpdateCounts or {})[name] or 0
end

function OnUpdate(deltaTime)
	updates = updates + 1
	_G.UpdateCounts = _G.UpdateCounts or {}
	_G.UpdateCounts["Controller"] = updates
end

function OnFixedUpdate()
	if updates == 0 or reported then return end
	checks = checks + 1

	-- Every script still running has had exactly one update per frame
	local running = { "Counter 2", "Counter 4" }
	if not _G.ScriptsRemoved then
		running = { "Counter 1", "Counter 2", "Counter 3", "Counter 4" }
	end
	for _, name in ipairs(running) do
		if Count(name) ~= updates then
			failures = failures + 1
			Test.Expect(false, "after " .. updates .. " frames, " .. name .. " has " .. Count(name) .. " updates")
		end
	end

	-- Removed scripts stop once the removal takes effect, at the end of the frame they were removed in
	if _G.ScriptsRemoved then
		if frozen == nil then
			frozen = { ["Counter 1"] = Count("Counter 1"), ["Counter 3"] = Count("Counter 3") }
		else
			for name, count in pairs(frozen) do
				if Count(name) ~= count then
					failures = failures + 1
					Test.Expect(false, name .. " kept updating after its script was removed (" .. count .. " -> " .. Count(name) .. ")")
				end
			end
		end
	end

	if updates >= 10 then
		reported = true
		if frozen == nil then
			Test.Fail("ScriptRemovalDuringUpdate: the scripts were never removed")
		else
			Test.Pass("ScriptRemovalDuringUpdate: " .. checks .. " checks, " .. failures .. " failed, removed scripts stopped at "
				.. frozen["Counter 1"] .. " and " .. frozen["Counter 3"] .. " updates")
		end
	end
end
