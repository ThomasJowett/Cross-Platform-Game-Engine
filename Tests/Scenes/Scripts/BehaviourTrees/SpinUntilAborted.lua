-- Yellow and spinning while running, turns red when the tree aborts it
function OnStateEntry()
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.9, 0.2, 1)
end

function OnStateUpdate(deltaTime)
	local transform = CurrentEntity:GetTransformComponent()
	local rotation = transform.Rotation
	transform.Rotation = Vec3.new(rotation.x, rotation.y, rotation.z + deltaTime * 3)
	return NodeStatus.Running
end

function OnStateExit(status)
	if status == NodeStatus.Aborted then
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
	end
end
