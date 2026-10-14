-- Destroys "Chain Target" from OnDestroy, i.e. while the engine is already destroying this entity.

function OnDestroy()
	local target = CurrentScene:FindEntity("Chain Target")
	if target:IsSceneValid() then
		Log.Info("DestroyParentChild: Child C's OnDestroy is destroying Chain Target")
		target:Destroy()
	end
end
