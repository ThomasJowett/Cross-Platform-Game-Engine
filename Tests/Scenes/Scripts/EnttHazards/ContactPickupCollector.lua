-- Falls onto a self-destroying Pickup and tracks begin/end contacts, like a ground or overlap counter.

local touching = 0

function OnBeginContact(other, normal, point)
	touching = touching + 1
	Log.Info("ContactDestroyedEntity: Collector began touching " .. other:GetName() .. " (touching " .. touching .. ")")
end

function OnEndContact(other)
	touching = touching - 1
	Log.Info("ContactDestroyedEntity: Collector OnEndContact, other valid = " .. tostring(other:IsSceneValid()) .. " (touching " .. touching .. ")")
end

function OnFixedUpdate()
	_G.CollectorTouching = touching
end
