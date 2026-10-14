-- Hazard: destroying the entity you are touching. Box2D's DestroyBody calls EndContact, which marks
-- the Contact2D old but keeps its freed fixture pointers; the next fixed update reads the entity
-- handle back out of the freed fixture to call OnEndContact. Reaching the end means no crash.
-- A second column has a Pickup that destroys itself under a Collector that counts begin/end contacts.

local fixedUpdates = 0
local destroyed = false

function OnBeginContact(other, normal, point)
	if destroyed then return end
	destroyed = true
	Log.Info("ContactDestroyedEntity: touched " .. other:GetName() .. ", destroying it")
	other:Destroy()
end

function OnEndContact(other)
	Log.Info("ContactDestroyedEntity: OnEndContact received, other valid = " .. tostring(other:IsSceneValid()))
	if other:IsSceneValid() then
		Log.Info("ContactDestroyedEntity: OnEndContact other is " .. other:GetName())
	end
end

function OnFixedUpdate()
	fixedUpdates = fixedUpdates + 1
	if fixedUpdates == 150 then
		Test.Expect(destroyed, "touched the floor")
		Test.Equal(_G.CollectorTouching, 0, "Collector count is back to 0")
		Test.Pass("ContactDestroyedEntity: survived destroying touching entities")
	end
end
