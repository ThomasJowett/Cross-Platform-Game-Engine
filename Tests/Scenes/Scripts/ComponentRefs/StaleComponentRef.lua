-- Repro: a component reference held in Lua goes stale when another entity is destroyed.
-- EnTT swap-and-pops component pools, so destroying A moves the last component (C) into A's
-- slot. The cached reference to C still points at the old slot, which the next new component (D) reuses.
-- Each circle's Radius is its ID tag (A=1, B=2, C=3, D=4).
-- A ref to a destroyed entity's component should report IsValid() == false and error when read.

local Tags = { [1] = "A", [2] = "B", [3] = "C", [4] = "D" }

local frame = 0
local entities = {}
local refs = {}

local function Spawn(name, x, radius, colour)
	local entity = CurrentScene:CreateEntity(name)
	local transform = entity:AddTransformComponent()
	transform.Position = Vec3.new(x, 0, 0)
	local circle = entity:AddCircleRendererComponent()
	circle.Radius = radius
	circle.Colour = colour
	entities[name] = entity
	return entity
end

local function Describe(ref)
	local tag = Tags[ref.Radius]
	return tag and ("entity " .. tag) or ("unknown data (Radius=" .. tostring(ref.Radius) .. ")")
end

-- Compares the cached reference against a freshly fetched one for the same entity.
local function CheckRef(name, stage)
	local fresh = entities[name]:GetCircleRendererComponent()
	local cached = refs[name]
	Test.Equal(cached.Radius, fresh.Radius, "[" .. stage .. "] cached ref for " .. name .. " reads " .. Describe(cached))
end

function OnCreate()
	Log.Info("StaleComponentRef: spawning A, B, C with CircleRenderers")
	Spawn("A", -6, 1, Colour.new(0.9, 0.2, 0.2, 1))
	Spawn("B", 0, 2, Colour.new(0.2, 0.9, 0.2, 1))
	Spawn("C", 6, 3, Colour.new(0.2, 0.4, 0.9, 1))
end

function OnUpdate(deltaTime)
	frame = frame + 1

	if frame == 1 then
		for _, name in ipairs({ "A", "B", "C" }) do
			refs[name] = entities[name]:GetCircleRendererComponent()
		end
		for _, name in ipairs({ "A", "B", "C" }) do
			CheckRef(name, "baseline")
		end
		Log.Info("StaleComponentRef: destroying A (deferred to end of frame)")
		entities["A"]:Destroy()

	elseif frame == 2 then
		-- The cached ref for A should now be dead, and reading it should raise an error.
		local ok, result = pcall(function() return Describe(refs["A"]) end)
		Test.Expect(not ok, "[destroyed A] reading cached ref for A raises" .. (ok and (", but it read " .. result) or ""))
		Test.Expect(not refs["A"]:IsValid(), "[destroyed A] cached ref for A reports IsValid() == false")

		CheckRef("B", "after destroying A")
		CheckRef("C", "after destroying A")

		Log.Info("StaleComponentRef: spawning D")
		Spawn("D", 12, 4, Colour.new(0.9, 0.9, 0.2, 1))
		CheckRef("C", "after spawning D")

		-- Write through the cached C ref; only C should turn white.
		refs["C"].Colour = Colour.new(1, 1, 1, 1)
		local cColour = entities["C"]:GetCircleRendererComponent().Colour
		local dColour = entities["D"]:GetCircleRendererComponent().Colour
		Test.Expect(cColour.r == 1 and cColour.g == 1 and cColour.b == 1, "[write] writing Colour through cached C ref changed C")
		Test.Expect(not (dColour.r == 1 and dColour.g == 1 and dColour.b == 1), "[write] writing Colour through cached C ref left D alone")

	elseif frame == 3 then
		Test.Pass("StaleComponentRef: checked the cached references")
	end
end
