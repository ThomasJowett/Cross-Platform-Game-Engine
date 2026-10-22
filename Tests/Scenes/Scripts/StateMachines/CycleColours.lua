-- Shared by the Red, Green and Blue states: shows this state's colour for one second, then changes to the next
local Colours = { Red = Colour.new(1, 0.2, 0.2, 1), Green = Colour.new(0.2, 1, 0.2, 1), Blue = Colour.new(0.3, 0.5, 1, 1) }
local Next = { Red = "Green", Green = "Blue", Blue = "Red" }

local name
local elapsed = 0

function OnStateEntry()
	name = StateMachine.GetCurrentStateName()
	elapsed = 0
	CurrentEntity:GetSpriteComponent().Tint = Colours[name]
end

function OnStateUpdate(deltaTime)
	elapsed = elapsed + deltaTime
	if elapsed >= 1 then
		StateMachine.ChangeState(Next[name])
	end
end
