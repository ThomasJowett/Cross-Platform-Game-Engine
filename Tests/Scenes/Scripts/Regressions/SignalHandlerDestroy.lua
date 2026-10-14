-- Signal.Emit loops over its subscriber list while running each handler. A handler that destroys
-- its own entity (which disconnects it immediately) or connects a new handler changes that list mid-loop.

local frame = 0
local listeners = 6

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 2 then
		Log.Info("SignalHandlerDestroy: emitting Boom to " .. listeners .. " listeners")
		Signal.Emit("Boom", CurrentEntity, {})
		Log.Info("SignalHandlerDestroy: Emit returned")
	elseif frame == 5 then
		local alive = 0
		for i = 1, listeners - 1 do
			if CurrentScene:FindEntity("Listener " .. i):IsSceneValid() then alive = alive + 1 end
		end
		if CurrentScene:FindEntity("Connector"):IsSceneValid() then alive = alive + 1 end
		Test.Equal(_G.BoomCalls, listeners, "every listener handled Boom once")
		Test.Equal(alive, 0, "every listener was destroyed")
		Test.Pass("SignalHandlerDestroy: checked the listeners")
	end
end
