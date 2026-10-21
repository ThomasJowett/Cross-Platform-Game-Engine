-- Plays an animation with events but defines neither callback, which must be harmless.

local update = 0

function OnUpdate(deltaTime)
	update = update + 1
end
