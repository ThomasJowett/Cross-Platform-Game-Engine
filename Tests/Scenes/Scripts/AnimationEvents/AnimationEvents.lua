-- Records every animation callback for two loops of "Attack" (4 frames of 0.05s) and checks the order, the
-- clip name and when each one arrived. Timing is measured in seconds on the second loop, so it also holds
-- with --display, where the first frame's deltaTime includes startup and can cross several frames at once.

local update = 0
local elapsed = 0
local deltas = {}
local received = {}
local done = false

-- An event recorded at update u fired during the next scene update, between elapsed and elapsed + deltas[u + 1]
local function Record(name, animationName)
	table.insert(received, { name = name, animation = animationName, update = update, time = elapsed })
end

function OnAnimationEvent(eventName, animationName)
	Record(eventName, animationName)
end

function OnAnimationFinished(animationName)
	Record("<Finished>", animationName)
end

local function Find(name, occurrence)
	local seen = 0
	for _, entry in ipairs(received) do
		if entry.name == name then
			seen = seen + 1
			if seen == (occurrence or 1) then return entry end
		end
	end
	return nil
end

-- Checks the time between two events, allowing for the length of the updates they fired in
local function Gap(from, to, expected, message)
	if not from or not to then
		Test.Expect(false, message .. " (event missing)")
		return
	end
	local actual = to.time - from.time
	local tolerance = math.max(deltas[from.update + 1], deltas[to.update + 1]) + 0.001
	Test.Expect(math.abs(actual - expected) <= tolerance,
		string.format("%s (expected %.3fs, got %.3fs, tolerance %.3fs)", message, expected, actual, tolerance))
end

function OnUpdate(deltaTime)
	update = update + 1
	deltas[update] = deltaTime
	elapsed = elapsed + deltaTime
	-- Two full loops take 0.4s; stop just after
	if done or elapsed < 0.42 then return end
	done = true

	local loop = { "Start", "Footstep", "Active", "Sound", "Footstep", "<Finished>" }
	local expected = {}
	for _ = 1, 2 do
		for _, name in ipairs(loop) do table.insert(expected, name) end
	end
	table.insert(expected, "Start")

	-- A long last update may cross into the third loop, so only the first two loops and a Start are compared
	local names = {}
	for i = 1, math.min(#received, #expected) do
		local entry = received[i]
		table.insert(names, entry.name)
		Test.Equal(entry.animation, "Attack", "'" .. entry.name .. "' names the clip that fired it")
	end
	Test.Equal(table.concat(names, ", "), table.concat(expected, ", "),
		"two loops of events in frame order, without the empty, duplicate or past-the-end events")

	Test.Equal(Find("Start").update, 0, "frame 0 fires on the first update, when the clip starts")

	local loopStart = Find("Start", 2)
	Gap(loopStart, Find("Footstep", 3), 0.05, "frame 1 fires 0.05s into the loop")
	Gap(loopStart, Find("Active", 2), 0.10, "frame 2 fires 0.10s into the loop")
	Gap(loopStart, Find("Footstep", 4), 0.15, "frame 3 fires 0.15s into the loop")
	Gap(loopStart, Find("<Finished>", 2), 0.20, "finished fires when the last frame's hold ends")
	Gap(Find("Start", 1), loopStart, 0.20, "frame 0 fires again when the clip loops")

	Test.Equal(Find("Active", 2).update, Find("Sound", 2).update, "both events on frame 2 arrive on the same update")
	Test.Equal(Find("<Finished>", 1).update, loopStart.update, "finished arrives on the same update as the loop's frame 0")

	Test.Pass("AnimationEvents: checked " .. #received .. " callbacks over " .. update .. " updates")
end
