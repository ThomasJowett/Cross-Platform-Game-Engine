TranslationSpeed = 5.0

local cameraComponent = {}
local transformComponent = {}

function walk( distance )
	local 
end

function OnCreate()
	cameraComponent = Entity:GetOrAddRigidBody2DComponent()
	transformComponent = Entity:GetTransformComponent()
end


function OnUpdate(deltaTime)
	local movement = Vec2.new()
	if(Input.IsKeyPressed('A'))
	{
		movement.x = movement.x - 1.0
	}

	if(Input.IsKeyPressed('D'))
	{
		movement.x = movement.x + 1.0
	}

	if(Input.IsKeyPressed('W'))
	{
		movement.y = movement.y + 1.0
	}

	if(Input.IsKeyPressed('S'))
	{
		movement.y = movement.y - 1.0
	}

	movement:Normalize()

	movement = movement * TranslationSpeed * deltaTime

end