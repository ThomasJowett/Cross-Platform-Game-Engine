-- Collision layer test: this entity should NOT touch the target below it.
-- Green = fell past the target with no OnBeginContact (pass), red = got a contact (fail).
-- Attach to a dynamic body with a collider and a Sprite or CircleRenderer.

local FreezeHeight = -5.0

local result = nil

local function SetColour(colour)
	local sprite = CurrentEntity:GetSpriteComponent()
	if sprite then sprite.Tint = colour end
	local circle = CurrentEntity:GetCircleRendererComponent()
	if circle then circle.Colour = colour end
end

-- Records this entity's result; the last entity to finish ends the test
local function Resolve(passed, message)
	Test.Expect(passed, message)
	_G.CollisionLayerPending = _G.CollisionLayerPending - 1
	if _G.CollisionLayerPending == 0 then
		Test.Pass("CollisionLayers: every entity finished")
	end
end

local function Freeze()
	local body = CurrentEntity:GetRigidBody2DComponent()
	body.GravityScale = 0
	body:SetLinearVelocity(Vec2.new(0, 0))
end

function OnCreate()
	SetColour(Colour.new(0.7, 0.7, 0.7, 1))
	_G.CollisionLayerPending = (_G.CollisionLayerPending or 0) + 1
end

function OnBeginContact(other, normal, point)
	if result ~= nil then return end

	result = false
	SetColour(Colour.new(0.9, 0.1, 0.1, 1))
	Resolve(false, CurrentEntity:GetName() .. " should not have touched " .. other:GetName())
end

function OnFixedUpdate()
	local position = CurrentEntity:GetTransformComponent().Position
	if position.y > FreezeHeight then return end

	Freeze()

	if result == nil then
		result = true
		SetColour(Colour.new(0.1, 0.9, 0.2, 1))
		Resolve(true, CurrentEntity:GetName() .. " passed through without a contact")
	end
end
