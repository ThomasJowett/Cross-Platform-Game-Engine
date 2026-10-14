-- An infinite loop in a script freezes the editor or game forever.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 2 then
		Log.Info("InfiniteLoop: entering while true do end")
		local ok, err = pcall(function() while true do end end)
		Log.Info("InfiniteLoop: loop was stopped: " .. tostring(err))
		Test.Expect(not ok, "runaway loop aborted with an error")
		Test.Pass("InfiniteLoop: the loop was stopped")
	end
end
