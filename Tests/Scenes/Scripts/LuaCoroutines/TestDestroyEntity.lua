-- A coroutine stops when its entity is destroyed, and never touches the dead entity.
require("Scripts/LuaCoroutines/CoroutineCommon")

local frame = 0

function OnCreate()
	StartCoroutine(function()
		while true do
			_G.DestroyEntityTicks = (_G.DestroyEntityTicks or 0) + 1
			CurrentEntity:GetName()
			coroutine.yield()
		end
	end)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 3 then
		_G.DestroyEntityTicksAtDestroy = _G.DestroyEntityTicks
		CurrentEntity:Destroy()
	end
end
