-- Watches the nine behaviour tree squares for 10 seconds and checks each one against the behaviour documented in
-- BehaviourTrees/README.md. A square shows its tree's state through its tint and rotation, so every frame this
-- records the tint as a run of one colour and whether the square rotated. Frame counts are at 100 frames a second. A change of colour and the last frame of spinning can land in the same
-- frame, so the "stays still" checks allow a few percent.

local Squares = { "Sequence", "Mem Sequence", "Selector", "Stateful Selector", "Parallel", "Decorators", "Expect Red", "New Tasks", "Halt" }
local Palette = {
	red = { 1, 0.2, 0.2 }, green = { 0.2, 1, 0.2 }, blue = { 0.3, 0.5, 1 }, white = { 1, 1, 1 }, yellow = { 1, 0.9, 0.2 },
}
local RunFrames = 1000

local frame = 0
local observed = {}

local function Classify(tint)
	for name, c in pairs(Palette) do
		if math.abs(tint.r - c[1]) < 0.05 and math.abs(tint.g - c[2]) < 0.05 and math.abs(tint.b - c[3]) < 0.05 then
			return name
		end
	end
	return "other"
end

-- Frames spent showing a colour
local function Frames(name, colour)
	local total = 0
	for _, run in ipairs(observed[name].runs) do
		if run.colour == colour then total = total + run.frames end
	end
	return total
end

-- Runs of a colour (or of any colour) that began and ended inside the recording, so their length is whole
local function CompleteRuns(name, colour)
	local runs = observed[name].runs
	local result = {}
	for i = 2, #runs - 1 do
		if colour == nil or runs[i].colour == colour then table.insert(result, runs[i]) end
	end
	return result
end

local function ExpectRunLengths(name, colour, minFrames, maxFrames)
	local runs = CompleteRuns(name, colour)
	Test.Expect(#runs >= 2, name .. ": saw at least 2 complete " .. colour .. " periods (saw " .. #runs .. ")")
	for _, run in ipairs(runs) do
		Test.Expect(run.frames >= minFrames and run.frames <= maxFrames,
			name .. ": a " .. colour .. " period lasted " .. run.frames .. " frames, expected " .. minFrames .. " to " .. maxFrames)
	end
end

-- Share of the frames showing a colour in which the square was rotating
local function SpinRatio(name, colour)
	local o = observed[name]
	local frames = o.spinCount[colour] or 0
	return frames == 0 and 0 or (o.spinning[colour] or 0) / frames
end

local function Check()
	for _, name in ipairs(Squares) do
		Test.Equal(Frames(name, "other"), 0, name .. ": only ever showed the palette colours")
	end

	-- Sequence restarts from SetRed every tick, so it stays red and only flashes green for a frame each second
	Test.Expect(Frames("Sequence", "red") >= 0.9 * RunFrames, "Sequence: stayed red (" .. Frames("Sequence", "red") .. " red frames)")
	Test.Equal(Frames("Sequence", "blue"), 0, "Sequence: never reached blue")
	local greenRuns = 0
	for _, run in ipairs(observed["Sequence"].runs) do
		if run.colour == "green" then
			greenRuns = greenRuns + 1
			Test.Expect(run.frames <= 2, "Sequence: a green flash lasted " .. run.frames .. " frames, expected at most 2")
		end
	end
	Test.Expect(greenRuns >= 8, "Sequence: flashed green about once a second (" .. greenRuns .. " flashes in 10 seconds)")

	-- MemSequence resumes where it was, so it walks red, green, blue for one second each
	local cycle = { "red", "green", "blue" }
	local memRuns = observed["Mem Sequence"].runs
	Test.Expect(#memRuns >= 9, "Mem Sequence: ran through at least 3 colour cycles (" .. #memRuns .. " periods)")
	for i = 2, #memRuns do
		Test.Equal(memRuns[i].colour, cycle[(i - 2) % 3 + 1], "Mem Sequence: period " .. (i - 1) .. " is the next colour in the cycle")
	end
	ExpectRunLengths("Mem Sequence", "red", 98, 103)
	ExpectRunLengths("Mem Sequence", "green", 98, 103)
	ExpectRunLengths("Mem Sequence", "blue", 98, 103)

	-- Selector re-runs the failing toggle every tick, so it swaps red and blue every frame
	local selectorRuns = observed["Selector"].runs
	local singles = 0
	for i = 2, #selectorRuns do
		if selectorRuns[i].frames == 1 then singles = singles + 1 end
	end
	Test.Expect(singles >= 0.95 * (#selectorRuns - 1), "Selector: changed colour every frame (" .. singles .. " of " .. (#selectorRuns - 1) .. " periods were one frame)")
	Test.Expect(Frames("Selector", "red") > 400 and Frames("Selector", "blue") > 400, "Selector: showed both red and blue throughout")

	-- StatefulSelector resumes at the Wait, so the toggle only runs once a second
	ExpectRunLengths("Stateful Selector", "red", 98, 104)
	ExpectRunLengths("Stateful Selector", "blue", 98, 104)

	-- ParallelSequence succeeds when the Wait does: white and spinning for 2 seconds, then yellow and still for 1
	ExpectRunLengths("Parallel", "white", 198, 203)
	ExpectRunLengths("Parallel", "yellow", 98, 103)
	Test.Expect(SpinRatio("Parallel", "white") >= 0.95, "Parallel: spun while white (" .. string.format("%.2f", SpinRatio("Parallel", "white")) .. " of frames)")
	Test.Expect(SpinRatio("Parallel", "yellow") <= 0.05, "Parallel: stayed still while yellow (" .. string.format("%.2f", SpinRatio("Parallel", "yellow")) .. " of frames)")

	-- Every decorator check passes, so the chain ends on SetGreen and the Selector never falls back to SetRed
	Test.Equal(Frames("Decorators", "red"), 0, "Decorators: never fell back to red")
	Test.Expect(Frames("Decorators", "green") >= RunFrames - 10, "Decorators: went green once the checks passed (" .. Frames("Decorators", "green") .. " green frames)")

	-- The first check in this tree is deliberately wrong, so it must always fall back to red
	Test.Equal(Frames("Expect Red", "green"), 0, "Expect Red: never turned green")
	Test.Expect(Frames("Expect Red", "red") >= RunFrames - 10, "Expect Red: stayed red (" .. Frames("Expect Red", "red") .. " red frames)")

	-- New Tasks alternates green (needs Set Blackboard) and blue (needs Emit Signal), each held for a Random Wait of 0.5 to 1.5 seconds
	local newTaskRuns = observed["New Tasks"].runs
	Test.Expect(#newTaskRuns >= 6, "New Tasks: alternated several times (" .. #newTaskRuns .. " periods)")
	for i = 2, #newTaskRuns do
		Test.Equal(newTaskRuns[i].colour, i % 2 == 0 and "green" or "blue", "New Tasks: period " .. (i - 1) .. " alternates green and blue")
	end
	ExpectRunLengths("New Tasks", "green", 48, 152)
	ExpectRunLengths("New Tasks", "blue", 48, 152)

	-- Halt: the Sequence re-checks its condition each tick, and when that fails the spin task is aborted and turns red
	ExpectRunLengths("Halt", "yellow", 198, 203)
	ExpectRunLengths("Halt", "red", 98, 103)
	Test.Expect(SpinRatio("Halt", "yellow") >= 0.95, "Halt: spun while yellow (" .. string.format("%.2f", SpinRatio("Halt", "yellow")) .. " of frames)")
	Test.Expect(SpinRatio("Halt", "red") <= 0.05, "Halt: stopped spinning once aborted (" .. string.format("%.2f", SpinRatio("Halt", "red")) .. " of frames)")
end

function OnCreate()
	for _, name in ipairs(Squares) do
		observed[name] = { entity = CurrentScene:FindEntity(name), runs = {}, spinCount = {}, spinning = {}, lastRotation = nil }
	end
end

function OnUpdate(deltaTime)
	frame = frame + 1
	for _, name in ipairs(Squares) do
		local o = observed[name]
		local colour = Classify(o.entity:GetSpriteComponent().Tint)
		local last = o.runs[#o.runs]
		if last and last.colour == colour then
			last.frames = last.frames + 1
		else
			table.insert(o.runs, { colour = colour, frames = 1 })
		end

		local rotation = o.entity:GetTransformComponent().Rotation.z
		if o.lastRotation ~= nil then
			o.spinCount[colour] = (o.spinCount[colour] or 0) + 1
			if rotation ~= o.lastRotation then o.spinning[colour] = (o.spinning[colour] or 0) + 1 end
		end
		o.lastRotation = rotation
	end

	if frame == RunFrames then
		Check()
		Test.Pass("BehaviourTrees: checked nine trees over " .. RunFrames / 100 .. " seconds")
	end
end
