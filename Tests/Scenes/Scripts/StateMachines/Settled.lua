-- End of the chain, turns blue
function OnStateEntry()
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.3, 0.5, 1, 1)
end
