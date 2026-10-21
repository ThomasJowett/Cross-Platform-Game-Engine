-- Checks AnimationEventDestroy a few frames after its "Destroy" event; reaching this means no crash.

local update = 0

function OnUpdate(deltaTime)
	update = update + 1
	if update ~= 10 then return end

	Test.Equal(_G.DestroyEventCount, 1, "the Destroy event fired once")
	Test.Expect(_G.AfterDestroyCount == nil, "no event reached the entity after it destroyed itself")
	Test.Expect(not CurrentScene:FindEntity("Destroyer"):IsSceneValid(), "the destroyer is gone")
	Test.Expect(CurrentScene:FindEntity("Spawned Sprite 8"):IsSceneValid(), "the sprites spawned during dispatch exist")

	local copyGotIdleStart = false
	for _, id in ipairs(_G.IdleStartEntities or {}) do
		if id == _G.SpawnedCopyId then copyGotIdleStart = true end
	end
	Test.Expect(copyGotIdleStart, "a copy spawned at runtime gets its clip's frame 0 event")

	Test.Pass("AnimationEventDestroy: destroyed and spawned during dispatch")
end
