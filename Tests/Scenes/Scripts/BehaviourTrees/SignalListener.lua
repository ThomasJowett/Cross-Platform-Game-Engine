-- Turns this entity blue when its own behaviour tree emits BTTestPing
function OnCreate()
	Signal.Connect("BTTestPing", CurrentEntity, function(sender, data)
		if sender == CurrentEntity then
			CurrentEntity:GetSpriteComponent().Tint = Colour.new(0.3, 0.5, 1, 1)
		end
	end)
end

function OnDestroy()
	Signal.Disconnect("BTTestPing", CurrentEntity)
end
