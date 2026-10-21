-- Records which entities received Idle's frame 0 event, so a copy made at runtime can be checked.

function OnAnimationEvent(eventName, animationName)
	if eventName == "IdleStart" then
		_G.IdleStartEntities = _G.IdleStartEntities or {}
		table.insert(_G.IdleStartEntities, CurrentEntity:GetID():ToString())
	end
end
