-- Reports the SelfRemoveScript result a few frames later; reaching it means no crash.
-- OnDestroy before then means the self-removal fired this script's OnDestroy by mistake.

local frame = 0
local myName = nil

function OnCreate()
	myName = CurrentEntity:GetName()
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 5 and myName == "Watcher" then
		_G.SelfRemoveDone = true
		Test.Expect(_G.SelfRemoved, "the script kept running after removing itself")
		Test.Expect(not CurrentScene:FindEntity("Self Remover"):HasLuaScriptComponent(), "the script was removed")
		Test.Pass("SelfRemoveScript: survived self-removal")
	end
end

function OnDestroy()
	if _G.SelfRemoveDone then return end
	Test.Fail("SelfRemoveScript: OnDestroy fired for " .. tostring(myName) .. ", which was not removed")
end
