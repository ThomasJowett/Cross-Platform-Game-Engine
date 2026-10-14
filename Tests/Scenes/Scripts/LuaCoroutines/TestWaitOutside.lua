-- Wait outside a coroutine raises a clear error instead of a confusing yield error.
require("Scripts/LuaCoroutines/CoroutineCommon")

function OnCreate()
	local ok, err = pcall(function() Wait(1) end)
	Report("WaitOutside", not ok and tostring(err):find("StartCoroutine") ~= nil, "pcall ok=" .. tostring(ok) .. " err=" .. tostring(err))
end
