-- Succeeds twice then fails, for testing Until Failure
local runs = 0

function OnStateUpdate(deltaTime)
	runs = runs + 1
	if runs % 3 == 0 then
		return NodeStatus.Failure
	end
	return NodeStatus.Success
end
