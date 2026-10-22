-- Shared by Ping and Pong, which change to each other on entry forever; the engine has to cut the loop off
function OnStateEntry()
	if StateMachine.GetCurrentStateName() == "Ping" then
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
		StateMachine.ChangeState("Pong")
	else
		CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.3, 0.5, 1, 1)
		StateMachine.ChangeState("Ping")
	end
end
