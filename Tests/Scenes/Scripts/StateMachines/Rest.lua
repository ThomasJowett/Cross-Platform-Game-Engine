-- Only defines OnStateUpdate: white and still for one second, then back to Spin
local elapsed = 0

function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 1, 1, 1)

	elapsed = elapsed + deltaTime
	if elapsed >= 1 then
		elapsed = 0
		StateMachine.ChangeState("Spin")
	end
end
