-- Collision layer test: checks the Lua Layer / Mask / CollidesWithLayer API.
-- Expects a BoxCollider2D on layer "Player" whose mask is exactly World + Pickup.
-- Green = every check passed, red = at least one failed.

local function SetColour(colour)
	local sprite = CurrentEntity:GetSpriteComponent()
	if sprite then sprite.Tint = colour end
end

function OnCreate()
	local collider = CurrentEntity:GetBoxCollider2DComponent()
	local failures = 0

	local function Check(condition, description)
		Test.Expect(condition, "Lua API - " .. description)
		if not condition then
			failures = failures + 1
		end
	end

	Check(collider.Layer == "Player", "Layer is 'Player' (got '" .. tostring(collider.Layer) .. "')")

	local inMask = {}
	local count = 0
	for _, name in ipairs(collider.Mask) do
		inMask[name] = true
		count = count + 1
	end
	Check(inMask["World"] and inMask["Pickup"] and count == 2, "Mask is exactly { World, Pickup } (got " .. count .. " names)")

	Check(collider:CollidesWithLayer("World"), "CollidesWithLayer('World') is true")
	Check(collider:CollidesWithLayer("Pickup"), "CollidesWithLayer('Pickup') is true")
	Check(not collider:CollidesWithLayer("Enemy"), "CollidesWithLayer('Enemy') is false")
	Check(not collider:CollidesWithLayer("Default"), "CollidesWithLayer('Default') is false")
	Check(not collider:CollidesWithLayer("NotALayer"), "CollidesWithLayer('NotALayer') is false")

	if failures == 0 then
		SetColour(Colour.new(0.1, 0.9, 0.2, 1))
	else
		SetColour(Colour.new(0.9, 0.1, 0.1, 1))
	end
end
