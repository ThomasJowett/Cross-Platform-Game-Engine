-- Rotates this entity for as long as it's ticked, never finishes on its own
function OnStateUpdate(deltaTime)
	local transform = CurrentEntity:GetTransformComponent()
	local rotation = transform.Rotation
	transform.Rotation = Vec3.new(rotation.x, rotation.y, rotation.z + deltaTime * 3)
	return NodeStatus.Running
end
