-- App.Quit() stops the game after the current frame: in the editor it ends Play mode without closing the
-- editor, and in an exported game it closes the application. Frame numbers are logged until it stops.
-- Under SceneTest it ends the run, so OnDestroy checks no frame ran after the quit.

local frame = 0

function OnUpdate(deltaTime)
	frame = frame + 1
	Log.Info("AppQuit: frame " .. frame)
	if frame == 3 then
		Log.Info("AppQuit: calling App.Quit()")
		App.Quit()
		Log.Info("AppQuit: App.Quit() returned; this frame still finishes")
	end
end

function OnDestroy()
	Log.Info("AppQuit: OnDestroy called after " .. frame .. " frames")
	Test.Equal(frame, 3, "no frame ran after App.Quit()")
	Test.Pass("AppQuit: the game stopped")
end
