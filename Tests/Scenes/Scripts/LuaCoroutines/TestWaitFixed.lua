-- WaitFixed resumes in the next fixed update, after this script's OnFixedUpdate.
require("Scripts/LuaCoroutines/CoroutineCommon")

local fixedSteps = 0
local resumedInFixedStep = nil

function OnFixedUpdate()
	fixedSteps = fixedSteps + 1
	if fixedSteps == 2 then
		StartCoroutine(function()
			WaitFixed()
			resumedInFixedStep = fixedSteps
		end)
	elseif fixedSteps == 10 then
		Report("WaitFixed", resumedInFixedStep == 3, "resumed in fixed step " .. tostring(resumedInFixedStep) .. ", expected 3")
	end
end
