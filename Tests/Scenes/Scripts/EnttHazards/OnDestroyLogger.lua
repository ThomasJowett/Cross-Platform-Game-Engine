-- Records every OnDestroy call in a shared table keyed by this entity's name.
-- The name is cached in OnCreate because components may already be gone when OnDestroy runs.

local myName = nil

function OnCreate()
	myName = CurrentEntity:GetName()
end

function OnUpdate(deltaTime)
	_G.UpdateCounts = _G.UpdateCounts or {}
	_G.UpdateCounts[myName] = (_G.UpdateCounts[myName] or 0) + 1
end

function OnDestroy()
	_G.DestroyCalls = _G.DestroyCalls or {}
	_G.DestroyCalls[myName] = (_G.DestroyCalls[myName] or 0) + 1
	Log.Info("OnDestroyWrongEntity: OnDestroy called for " .. tostring(myName))
end
