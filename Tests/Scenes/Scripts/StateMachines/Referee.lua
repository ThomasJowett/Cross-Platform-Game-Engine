-- Watches the five state machine squares for 10 seconds and checks each one. A square shows its state through its
-- tint and rotation, so every frame this records the tint as a run of one colour, how long the run lasted, and the
-- state machine's current and previous state names. Durations are measured in seconds rather than frames so the
-- test also passes when run with --display at the monitor's frame rate. Without --fixed-step the first frame's
-- deltaTime includes startup, long enough for states to time out at once, so the first frames aren't recorded.

local Squares = { "Cycle", "Spin", "Switch", "Chain", "Runaway" }
local Palette = {
	red = { 1, 0.2, 0.2 }, green = { 0.2, 1, 0.2 }, blue = { 0.3, 0.5, 1 }, white = { 1, 1, 1 }, yellow = { 1, 0.9, 0.2 },
}
local RunTime = 10
local Tolerance = 0.1
local SkippedFrames = 2

local elapsed = 0
local frame = 0
local finished = false
local observed = {}

local function Classify(tint)
	for name, c in pairs(Palette) do
		if math.abs(tint.r - c[1]) < 0.05 and math.abs(tint.g - c[2]) < 0.05 and math.abs(tint.b - c[3]) < 0.05 then
			return name
		end
	end
	return "other"
end

local function Frames(name, colour)
	local total = 0
	for _, run in ipairs(observed[name].runs) do
		if run.colour == colour then total = total + run.frames end
	end
	return total
end

-- Runs of a colour that began and ended inside the recording, so their length is whole
local function CompleteRuns(name, colour)
	local runs = observed[name].runs
	local result = {}
	for i = 2, #runs - 1 do
		if runs[i].colour == colour then table.insert(result, runs[i]) end
	end
	return result
end

local function ExpectRunTimes(name, colour, seconds)
	local runs = CompleteRuns(name, colour)
	Test.Expect(#runs >= 2, name .. ": saw at least 2 complete " .. colour .. " periods (saw " .. #runs .. ")")
	for _, run in ipairs(runs) do
		Test.Expect(math.abs(run.time - seconds) <= Tolerance,
			name .. ": a " .. colour .. " period lasted " .. string.format("%.2f", run.time) .. "s, expected " .. seconds .. "s")
	end
end

-- Every run must be the colour after the previous run's in the cycle
local function ExpectCycle(name, cycle)
	local runs = observed[name].runs
	Test.Expect(#runs >= #cycle + 1, name .. ": went round its colours at least once (" .. #runs .. " periods)")
	local next = {}
	for i, colour in ipairs(cycle) do
		next[colour] = cycle[i % #cycle + 1]
	end
	for i = 2, #runs do
		Test.Equal(runs[i].colour, next[runs[i - 1].colour], name .. ": period " .. i .. " is the next colour after " .. runs[i - 1].colour)
	end
end

local function Check()
	for _, name in ipairs(Squares) do
		Test.Equal(Frames(name, "other"), 0, name .. ": only ever showed the palette colours")
	end

	-- Cycle changes state from OnStateUpdate after one second: Red, Green, Blue, repeating
	ExpectCycle("Cycle", { "red", "green", "blue" })
	ExpectRunTimes("Cycle", "red", 1)
	ExpectRunTimes("Cycle", "green", 1)
	ExpectRunTimes("Cycle", "blue", 1)
	Test.Equal(observed["Cycle"].nameMismatches, 0, "Cycle: GetCurrentStateName always matched the colour shown")
	Test.Equal(observed["Cycle"].previousMismatches, 0, "Cycle: GetPreviousStateName was always the state before")

	-- Spin is yellow and spinning for 2 seconds, then Rest (OnStateUpdate only) is white and still for 1. Spin's OnStateExit
	-- puts the rotation back to zero, so it must be exactly zero whenever the square is white
	ExpectCycle("Spin", { "yellow", "white" })
	ExpectRunTimes("Spin", "yellow", 2)
	ExpectRunTimes("Spin", "white", 1)
	local spin = observed["Spin"]
	Test.Expect(spin.spinning.yellow / spin.frames.yellow >= 0.95, "Spin: spun while yellow (" .. spin.spinning.yellow .. " of " .. spin.frames.yellow .. " frames)")
	Test.Equal(spin.spinning.white, 0, "Spin: stayed still while white")
	Test.Equal(spin.rotatedWhileWhite, 0, "Spin: OnStateExit reset the rotation before every rest")

	-- Switch is driven by a gameplay script through GetStateMachineComponent, flipping Off (red) and On (green) every half second
	ExpectCycle("Switch", { "red", "green" })
	ExpectRunTimes("Switch", "red", 0.5)
	ExpectRunTimes("Switch", "green", 0.5)

	-- Chain changes state from OnStateEntry twice in a row, so it lands in Settled (blue) on the first update and never runs
	-- the OnStateUpdate that would turn it red
	Test.Equal(Frames("Chain", "red"), 0, "Chain: skipped straight through Start and Bounce")
	Test.Equal(Frames("Chain", "blue"), frame - SkippedFrames, "Chain: blue from its first update")
	local chain = observed["Chain"].entity:GetStateMachineComponent()
	Test.Equal(chain:GetCurrentStateName(), "Settled", "Chain: ended in Settled")
	Test.Equal(chain:GetPreviousStateName(), "Bounce", "Chain: came to Settled from Bounce")

	-- Runaway's two states change to each other on entry forever; the engine stops the loop in one of them
	local runaway = observed["Runaway"]
	Test.Expect(runaway.firstName == "Ping" or runaway.firstName == "Pong", "Runaway: stopped in Ping or Pong (" .. tostring(runaway.firstName) .. ")")
	Test.Equal(runaway.nameChanges, 0, "Runaway: stayed in the state the loop was stopped in")
	Test.Equal(runaway.nameMismatches, 0, "Runaway: its colour matches the state it stopped in")
end

local StateColours = { Red = "red", Green = "green", Blue = "blue" }
local PreviousStates = { Red = "Blue", Green = "Red", Blue = "Green" }

local function Observe(name, o, colour, deltaTime)
	local stateMachine = o.entity:GetStateMachineComponent()
	local current = stateMachine:GetCurrentStateName()

	if name == "Cycle" and colour ~= "white" then
		if StateColours[current] ~= colour then o.nameMismatches = o.nameMismatches + 1 end
		-- The very first state has no previous state
		local previous = stateMachine:GetPreviousStateName()
		if previous ~= "" and previous ~= PreviousStates[current] then o.previousMismatches = o.previousMismatches + 1 end
	elseif name == "Runaway" then
		if o.firstName == nil then o.firstName = current end
		if current ~= o.firstName then o.nameChanges = o.nameChanges + 1 end
		local expected = current == "Ping" and "red" or "blue"
		if colour ~= expected then o.nameMismatches = o.nameMismatches + 1 end
	elseif name == "Spin" then
		local rotation = o.entity:GetTransformComponent().Rotation.z
		if o.lastRotation ~= nil then
			o.frames[colour] = (o.frames[colour] or 0) + 1
			if rotation ~= o.lastRotation then o.spinning[colour] = (o.spinning[colour] or 0) + 1 end
		end
		if colour == "white" and rotation ~= 0 then o.rotatedWhileWhite = o.rotatedWhileWhite + 1 end
		o.lastRotation = rotation
	end
end

function OnCreate()
	for _, name in ipairs(Squares) do
		observed[name] = {
			entity = CurrentScene:FindEntity(name), runs = {}, nameMismatches = 0, previousMismatches = 0, nameChanges = 0,
			frames = { yellow = 0, white = 0 }, spinning = { yellow = 0, white = 0 }, rotatedWhileWhite = 0,
		}
	end
end

function OnUpdate(deltaTime)
	if finished then
		return
	end

	frame = frame + 1
	elapsed = elapsed + deltaTime
	if frame <= SkippedFrames then
		return
	end

	for _, name in ipairs(Squares) do
		local o = observed[name]
		local colour = Classify(o.entity:GetSpriteComponent().Tint)
		local last = o.runs[#o.runs]
		if last and last.colour == colour then
			last.frames = last.frames + 1
			last.time = last.time + deltaTime
		else
			table.insert(o.runs, { colour = colour, frames = 1, time = deltaTime })
		end
		Observe(name, o, colour, deltaTime)
	end

	if elapsed >= RunTime then
		finished = true
		Check()
		Test.Pass("StateMachines: checked five state machines over " .. RunTime .. " seconds")
	end
end
