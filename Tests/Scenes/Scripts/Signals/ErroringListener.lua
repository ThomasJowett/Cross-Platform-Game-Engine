-- Handles "Ping" with an error, which should be reported against this file (and its line) in the error panel.

function OnCreate()
	Signal.Connect("Ping", CurrentEntity, function(sender, data)
		error("deliberate signal handler error")
	end)
end
