-- Reports RemoveOwnScript's result once the removal has had time to happen.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 5 then
		local remover = CurrentScene:FindEntity("Self Remover")
		Test.Expect(_G.RemoveOwnScriptFinished, "the script finished its update after removing itself")
		Test.Expect(not remover:HasLuaScriptComponent(), "the script was removed")
		Test.Pass("RemoveOwnScript: checked the removal")
	end
end
