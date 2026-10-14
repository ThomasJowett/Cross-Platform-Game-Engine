-- Listens for "Boom" and destroys itself in the handler, the usual "die on game over" pattern.
-- The listener named "Connector" also connects a new handler from inside its handler.

function OnCreate()
	local name = CurrentEntity:GetName()
	Signal.Connect("Boom", CurrentEntity, function(sender, data)
		_G.BoomCalls = (_G.BoomCalls or 0) + 1
		Log.Info("SignalHandlerDestroy: " .. name .. " got Boom, destroying itself")
		if name == "Connector" then
			Signal.Connect("Boom", CurrentEntity, function() end)
		end
		CurrentEntity:Destroy()
	end)
end
