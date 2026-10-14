-- Counts how many times a Repeater ran it, in the RepeatCount blackboard int
function OnStateUpdate(deltaTime)
	local count = Blackboard:GetInt("RepeatCount") + 1
	Blackboard:SetInt("RepeatCount", count)
	Log.Info(CurrentEntity:GetName() .. ": Repeater tick " .. count)
	return NodeStatus.Success
end
