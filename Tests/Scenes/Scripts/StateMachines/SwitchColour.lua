-- Shared by the Off and On states, which never change on their own: red when Off, green when On
function OnStateEntry()
	if StateMachine.GetCurrentStateName() == "On" then
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.2, 1, 0.2, 1)
	else
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
	end
end
