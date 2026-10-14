-- Tints this entity green and succeeds
function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.2, 1, 0.2, 1)
	return NodeStatus.Success
end
