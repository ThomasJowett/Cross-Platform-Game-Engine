-- Tints this entity white and succeeds
function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 1, 1, 1)
	return NodeStatus.Success
end
