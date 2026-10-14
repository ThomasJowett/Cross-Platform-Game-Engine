-- Hazard: Lua uses an Entity that was destroyed (its index then recycled), or one FindEntity didn't find.
-- Each misuse should raise a Lua error (or be a harmless no-op) rather than assert or crash the engine.
-- Each step logs before it runs, so if the editor crashes the last line shows which call did it.

local frame = 0
local target = nil
local targetTransform = nil

-- Runs fn in pcall and checks whether it raised an error as expected.
local function Expect(description, shouldError, fn)
	Log.Info("StaleEntityHandle: " .. description)
	local ok, result = pcall(fn)
	Test.Expect(ok == not shouldError, description .. (ok and (" -> returned " .. tostring(result)) or (" -> raised: " .. tostring(result))))
end

function OnCreate()
	-- The two uncaught errors in frames 3 and 4
	Test.ExpectErrors(2)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 1 then
		target = CurrentScene:CreateEntity("Target")
		targetTransform = target:AddTransformComponent()
		Log.Info("StaleEntityHandle: destroying Target")
		target:Destroy()
	elseif frame == 2 then
		local recycled = CurrentScene:CreateEntity("Recycled")
		recycled:AddTransformComponent()
		local missing = CurrentScene:FindEntity("Does Not Exist")

		Test.Expect(not target:IsSceneValid(), "destroyed Target reports IsSceneValid() == false")

		Expect("Destroy() again on the destroyed Target is a no-op", false, function() target:Destroy() end)
		Expect("Destroy() on a FindEntity result that wasn't found is a no-op", false, function() missing:Destroy() end)
		Expect("GetName() on the destroyed Target raises", true, function() return target:GetName() end)
		Expect("GetName() on a FindEntity result that wasn't found raises", true, function() return missing:GetName() end)
		Expect("SetName() on the destroyed Target raises", true, function() target:SetName("Renamed") end)
		Expect("GetID() on the destroyed Target raises", true, function() return target:GetID() end)
		Expect("GetParent() on the destroyed Target raises", true, function() return target:GetParent() end)
		Expect("GetTransformComponent() on the destroyed Target raises", true, function() return target:GetTransformComponent() end)
		Expect("HasTransformComponent() on the destroyed Target raises", true, function() return target:HasTransformComponent() end)
		Expect("AddSpriteComponent() on the destroyed Target raises", true, function() return target:AddSpriteComponent() end)
		Expect("recycled:AddChild(destroyed Target) raises", true, function() recycled:AddChild(target) end)
		Expect("CurrentScene:InstantiateEntity(destroyed Target) raises", true, function() return CurrentScene:InstantiateEntity(target, Vec3.new(0, 0, 0)) end)
		Expect("Pathfinding.FindPath with a destroyed tilemap entity returns an empty path", false, function()
			local path = Pathfinding.FindPath(Vec2.new(0, 0), Vec2.new(1, 1), target)
			if #path ~= 0 then error("expected an empty path") end
		end)

		Expect("AddTransformComponent() when it already has one raises", true, function() return recycled:AddTransformComponent() end)
		Expect("RemoveSpriteComponent() when it has none raises", true, function() recycled:RemoveSpriteComponent() end)
		Expect("Recycled still works after all that", false, function()
			if recycled:GetName() ~= "Recycled" then error("name is " .. recycled:GetName()) end
		end)

	elseif frame == 3 then
		-- Uncaught on purpose: the engine should report these with this file's line numbers
		Log.Info("StaleEntityHandle: uncaught GetName() on the destroyed Target, expect an error on the next line")
		target:GetName()
	elseif frame == 4 then
		Log.Info("StaleEntityHandle: uncaught read of a destroyed Target's TransformComponent, expect an error on the next line")
		local position = targetTransform.Position
	elseif frame == 5 then
		Test.Pass("StaleEntityHandle: every misuse was handled")
	end
end
