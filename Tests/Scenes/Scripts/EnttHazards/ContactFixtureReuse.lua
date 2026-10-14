-- Hazard: after Floor's body is destroyed, its Contact2D keeps the freed fixture pointer. Box2D reuses
-- that memory for the next fixture created, so OnEndContact can report the newly spawned entity
-- instead of Floor. Floor is destroyed and Replacement spawned in the same OnUpdate so the timing is fixed.

local touched = false
local handled = false
local endContacts = {}
local fixedUpdates = 0

function OnBeginContact(other, normal, point)
	if other:GetName() == "Floor" then
		touched = true
	end
end

function OnEndContact(other)
	local name = other:IsSceneValid() and other:GetName() or "<destroyed entity>"
	table.insert(endContacts, name)
	Log.Info("ContactFixtureReuse: OnEndContact other = " .. name)
end

function OnUpdate(deltaTime)
	if touched and not handled then
		handled = true
		Log.Info("ContactFixtureReuse: destroying Floor and spawning Replacement far away")
		CurrentScene:FindEntity("Floor"):Destroy()
		local replacement = CurrentScene:CreateEntity("Replacement")
		replacement:AddTransformComponent().Position = Vec3.new(30, 0, 0)
		replacement:AddRigidBody2DComponent().Type = BodyType.Static
		replacement:AddBoxCollider2DComponent()
	end
end

function OnFixedUpdate()
	fixedUpdates = fixedUpdates + 1
	if fixedUpdates == 150 then
		local wrong = 0
		for _, name in ipairs(endContacts) do
			if name == "Replacement" then wrong = wrong + 1 end
		end
		Test.Expect(handled, "touched the floor")
		Test.Equal(wrong, 0, "OnEndContact never reported Replacement, which was never touched")
		Test.Pass("ContactFixtureReuse: " .. #endContacts .. " OnEndContact call(s)")
	end
end
