-- Fails twice then succeeds, for testing Until Success
local runs = 0

function OnStateUpdate(deltaTime)
	runs = runs + 1
	if runs % 3 == 0 then
		return NodeStatus.Success
	end
	return NodeStatus.Failure
end
