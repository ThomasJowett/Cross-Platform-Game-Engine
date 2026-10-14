-- Sol2 only checks argument and self types in unoptimised builds. In Release, common mistakes
-- such as calling with "." instead of ":" or passing nil reach C++ as null pointers and crash.
-- Each mistake runs in pcall and should raise a Lua error in every build.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame ~= 2 then return end

	local body = CurrentEntity:GetRigidBody2DComponent()
	local transform = CurrentEntity:GetTransformComponent()
	local steps = {
		{ "CurrentEntity.GetName() (dot instead of colon)", function() return CurrentEntity.GetName() end },
		{ "body.ApplyTorque(1) (dot instead of colon)", function() body.ApplyTorque(1) end },
		{ "body:SetLinearVelocity(nil)", function() body:SetLinearVelocity(nil) end },
		{ "transform.Position = nil", function() transform.Position = nil end },
		{ "CurrentEntity:AddChild(nil)", function() CurrentEntity:AddChild(nil) end },
		{ "CurrentScene:InstantiateEntity(nil, nil)", function() CurrentScene:InstantiateEntity(nil, nil) end },
	}

	for _, step in ipairs(steps) do
		Log.Info("WrongArgTypes: " .. step[1])
		local ok, err = pcall(step[2])
		Test.Expect(not ok, step[1] .. " raises an error" .. (ok and "" or (": " .. tostring(err))))
	end

	Test.Pass("WrongArgTypes: checked " .. #steps .. " bad calls")
end
