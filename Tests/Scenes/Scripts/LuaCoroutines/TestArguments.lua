-- Extra StartCoroutine arguments are passed to the function.
require("Scripts/LuaCoroutines/CoroutineCommon")

function OnCreate()
	local received = nil
	StartCoroutine(function(a, b) received = tostring(a) .. "+" .. tostring(b) end, "x", 2)
	Report("Arguments", received == "x+2", "received " .. tostring(received))
end
