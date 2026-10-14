-- An error in a signal handler is logged and sent to the error panel with the handler's file and line,
-- and the other handlers still run. The log line names the file; the error panel shows it too.

local frame = 0

function OnCreate()
	-- ErroringListener's deliberate handler error
	Test.ExpectErrors(1)
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 2 then
		Signal.Emit("Ping", CurrentEntity, {})
	elseif frame == 4 then
		Test.Equal(_G.PingsReceived, 1, "the other handler still ran once")
		Test.Pass("SignalHandlerError: emitted Ping")
	end
end
