-- On its "Destroy" event this entity spawns entities with animated sprites (adding to the pool being
-- dispatched from) and destroys itself. "AfterDestroy", on the same frame, must not reach the destroyed entity.

function OnAnimationEvent(eventName, animationName)
	if eventName == "Destroy" then
		_G.DestroyEventCount = (_G.DestroyEventCount or 0) + 1
		local sheet = CurrentEntity:GetAnimatedSpriteComponent().SpriteSheet

		for i = 1, 8 do
			local spawned = CurrentScene:CreateEntity("Spawned Sprite " .. i)
			spawned:AddTransformComponent()
			local sprite = spawned:AddAnimatedSpriteComponent()
			sprite.SpriteSheet = sheet
			sprite.Animation = "Attack"
		end

		local copy = CurrentScene:InstantiateEntity(CurrentScene:FindEntity("Template"), Vec3.new(4, 0, 0))
		copy:GetAnimatedSpriteComponent().Animation = "Idle"
		_G.SpawnedCopyId = copy:GetID():ToString()

		CurrentEntity:Destroy()
	elseif eventName == "AfterDestroy" then
		_G.AfterDestroyCount = (_G.AfterDestroyCount or 0) + 1
	end
end
