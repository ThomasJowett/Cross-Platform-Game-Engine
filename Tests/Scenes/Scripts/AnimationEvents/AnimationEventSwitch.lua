-- Switches from "Attack" to "Idle" in OnAnimationFinished. "Idle" covers the same frames as "Attack", so only
-- the change of name restarts it; its frame 0 event must fire next and no "Attack" event may follow, not even
-- the "Start" of the loop that began on the same update.

local update = 0
local switchedAt = nil
local idleStarts = 0
local idleFinishes = 0

function OnAnimationEvent(eventName, animationName)
	if switchedAt then
		Test.Equal(animationName, "Idle", "'" .. eventName .. "' after the switch comes from Idle")
		if eventName == "IdleStart" then
			idleStarts = idleStarts + 1
			if idleStarts == 1 then
				Test.Equal(update - switchedAt, 1, "Idle's frame 0 fires on the update after the switch")
			end
		end
	end
end

function OnAnimationFinished(animationName)
	if animationName == "Attack" then
		Test.Expect(switchedAt == nil, "Attack finishes only once")
		switchedAt = update
		CurrentEntity:GetAnimatedSpriteComponent().Animation = "Idle"
	elseif animationName == "Idle" then
		idleFinishes = idleFinishes + 1
	end
end

function OnUpdate(deltaTime)
	update = update + 1
	if update == 60 then
		Test.Expect(switchedAt ~= nil, "Attack finished")
		-- Idle loops every 0.1s, so about four loops after the switch at update 20
		Test.Expect(idleStarts >= 3, "Idle restarted at frame 0 and kept looping (" .. idleStarts .. " starts)")
		Test.Expect(idleFinishes >= 3, "Idle finished once per loop (" .. idleFinishes .. " finishes)")
		Test.Pass("AnimationEventSwitch: switched clips from OnAnimationFinished")
	end
end
