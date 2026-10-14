-- Counts "Ping", to show one failing handler doesn't stop the others.

function OnCreate()
	Signal.Connect("Ping", CurrentEntity, function(sender, data)
		_G.PingsReceived = (_G.PingsReceived or 0) + 1
	end)
end
