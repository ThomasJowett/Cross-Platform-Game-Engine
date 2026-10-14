-- Swaps between red and blue every time it runs, then fails so a selector moves on
local isRed = false

function OnStateUpdate(deltaTime)
	isRed = not isRed
	if isRed then
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
	else
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.3, 0.5, 1, 1)
	end
	return NodeStatus.Failure
end
