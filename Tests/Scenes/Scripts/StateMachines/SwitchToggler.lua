-- Gameplay script driving its own entity's state machine: flips it between Off and On every half second
local elapsed = 0
local flips = 0

function OnUpdate(deltaTime)
	elapsed = elapsed + deltaTime
	if elapsed < 0.5 then
		return
	end
	elapsed = 0

	local stateMachine = CurrentEntity:GetStateMachineComponent()
	local current = stateMachine:GetCurrentStateName()
	local target = current == "On" and "Off" or "On"

	Test.Expect(stateMachine:ChangeState(target), "Switch: ChangeState(\"" .. target .. "\") from a gameplay script succeeded")
	Test.Equal(stateMachine:GetCurrentStateName(), target, "Switch: changed state straight away")
	Test.Equal(stateMachine:GetPreviousStateName(), current, "Switch: previous state is the one it left")

	flips = flips + 1
	if flips == 1 then
		Test.Equal(stateMachine:ChangeState("Missing"), false, "Switch: ChangeState to a state that doesn't exist returns false")
		Test.Equal(stateMachine:GetCurrentStateName(), target, "Switch: an unknown state leaves the current state alone")
	end
end
