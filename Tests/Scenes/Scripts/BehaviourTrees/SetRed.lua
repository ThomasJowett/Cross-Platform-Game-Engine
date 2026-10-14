-- Tints this entity red and succeeds
function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
	return NodeStatus.Success
end
