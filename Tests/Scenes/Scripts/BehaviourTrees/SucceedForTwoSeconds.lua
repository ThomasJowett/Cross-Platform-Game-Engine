-- Succeeds for 2 seconds, then fails for 1, repeating
local elapsed = 0

function OnStateUpdate(deltaTime)
	elapsed = elapsed + deltaTime
	if elapsed % 3 < 2 then
		return NodeStatus.Success
	end
	return NodeStatus.Failure
end
