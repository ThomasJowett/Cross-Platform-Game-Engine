-- Collision layer test: this entity SHOULD touch the target below it.
-- Green = got OnBeginContact (pass), red = fell past the target without one (fail).
-- Attach to a dynamic body with a collider and a Sprite or CircleRenderer.

local FreezeHeight = -5.0

local result = nil

local function SetColour(colour)
	local sprite = CurrentEntity:GetSpriteComponent()
	if sprite then sprite.Tint = colour end
	local circle = CurrentEntity:GetCircleRendererComponent()
	if circle then circle.Colour = colour end
end

local function GetCollider(entity)
	return entity:GetBoxCollider2DComponent()
		or entity:GetCircleCollider2DComponent()
		or entity:GetCapsuleCollider2DComponent()
		or entity:GetPolygonCollider2DComponent()
		or entity:GetTilemapComponent()
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
	if result then return end

	local otherCollider = GetCollider(other)
	local otherLayer = otherCollider and otherCollider.Layer or "?"

	result = true
	SetColour(Colour.new(0.1, 0.9, 0.2, 1))
	Resolve(true, CurrentEntity:GetName() .. " touched " .. other:GetName() .. " (layer " .. otherLayer .. ")")
end

function OnFixedUpdate()
	local position = CurrentEntity:GetTransformComponent().Position
	if position.y > FreezeHeight then return end

	Freeze()

	if result == nil then
		result = false
		SetColour(Colour.new(0.9, 0.1, 0.1, 1))
		Resolve(false, CurrentEntity:GetName() .. " expected a contact but passed straight through")
	end
end
