-- Tints this entity blue and succeeds
function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.3, 0.5, 1, 1)
	return NodeStatus.Success
end
