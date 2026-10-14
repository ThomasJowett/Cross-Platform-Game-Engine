-- A pickup that destroys itself when touched, the common coin/collectable pattern.

function OnBeginContact(other, normal, point)
	Log.Info("ContactDestroyedEntity: Pickup touched by " .. other:GetName() .. ", destroying itself")
	CurrentEntity:Destroy()
end
