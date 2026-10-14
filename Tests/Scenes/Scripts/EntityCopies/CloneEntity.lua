-- Copies made with InstantiateEntity (which shares its copying with editor Duplicate) must get new UUIDs,
-- their own physics body and their own running scripts - including on child entities.

local frame = 0
local source, sourceChild, copy, copyChild

local function Check(description, passed, detail)
	Test.Expect(passed, description .. (detail and (" - " .. detail) or ""))
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 2 then
		source = CurrentScene:FindEntity("Source")
		sourceChild = source:GetChild()
		copy = CurrentScene:InstantiateEntity(source, Vec3.new(6, 0, 0))
		copyChild = copy:GetChild()

		Check("copy has a new UUID", copy:GetID() ~= source:GetID())
		Check("copy's child exists", copyChild:IsSceneValid())
		Check("copy's child has a new UUID", copyChild:IsSceneValid() and copyChild:GetID() ~= sourceChild:GetID())
	elseif frame == 4 then
		local ids = _G.ChildScriptEntities or {}
		local copyChildId = copyChild:GetID():ToString()
		local copyRan = false
		for _, id in ipairs(ids) do
			if id == copyChildId then copyRan = true end
		end
		Check("the copied child's script ran for the copy", copyRan, #ids .. " script(s) created")
	elseif frame == 6 then
		Log.Info("CloneEntity: destroying the source")
		source:Destroy()
	elseif frame == 30 then
		Check("copy still exists after the source was destroyed", copy:IsSceneValid())
		local velocity = copy:GetRigidBody2DComponent():GetLinearVelocity()
		Check("copy has its own simulated body", velocity.y < 0, "velocity.y=" .. velocity.y)

		Test.Pass("CloneEntity: checked the copy")
	end
end
