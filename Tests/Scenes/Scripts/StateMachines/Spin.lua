-- Yellow and spinning for two seconds, then rests; leaving puts the rotation back to zero
local elapsed = 0

function OnStateEntry()
	elapsed = 0
	CurrentEntity:GetSpriteComponent().Tint = Colour.new(1, 0.9, 0.2, 1)
end

function OnStateUpdate(deltaTime)
	local transform = CurrentEntity:GetTransformComponent()
	local rotation = transform.Rotation
	transform.Rotation = Vec3.new(rotation.x, rotation.y, rotation.z + deltaTime * 3)

	elapsed = elapsed + deltaTime
	if elapsed >= 2 then
		StateMachine.ChangeState("Rest")
	end
end

function OnStateExit()
	local transform = CurrentEntity:GetTransformComponent()
	local rotation = transform.Rotation
	transform.Rotation = Vec3.new(rotation.x, rotation.y, 0)
end
