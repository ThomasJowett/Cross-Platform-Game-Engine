-- Shared by Start and Bounce: changes state as soon as it's entered, so its OnStateUpdate must never run
local Next = { Start = "Bounce", Bounce = "Settled" }

function OnStateEntry()
	StateMachine.ChangeState(Next[StateMachine.GetCurrentStateName()])
end

function OnStateUpdate(deltaTime)
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.2, 0.2, 1)
end
