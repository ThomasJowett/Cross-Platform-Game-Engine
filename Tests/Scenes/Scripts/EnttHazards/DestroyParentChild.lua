-- Hazard: Scene's end-of-frame flush iterates the DestroyMarker view while SceneGraph::Remove
-- destroys children that are also in that view. Pair A marks the child before the parent, pair B
-- marks the parent first. Parent C's child destroys "Chain Target" from its OnDestroy, during the flush.
-- Reaching the end means the flush survived.

local frame = 0
local entities = {}


function OnCreate()
	for _, name in ipairs({ "Parent A", "Parent A/Child A", "Parent B", "Parent B/Child B", "Parent C", "Parent C/Child C", "Chain Target" }) do
		entities[name] = CurrentScene:FindEntity(name)
		Test.Expect(entities[name]:IsSceneValid(), "found " .. name)
		if entities[name]:IsSceneValid() then
			Test.Equal(entities[name]:GetName(), name:match("[^/]+$"), "FindEntity(\"" .. name .. "\") returns the right entity")
		end
	end
end

function OnUpdate(deltaTime)
	frame = frame + 1
	if frame == 1 then
		Log.Info("DestroyParentChild: marking Child A, Parent A, Parent B, Child B in that order")
		entities["Parent A/Child A"]:Destroy()
		entities["Parent A"]:Destroy()
		entities["Parent B"]:Destroy()
		entities["Parent B/Child B"]:Destroy()
		entities["Parent C"]:Destroy()
	elseif frame == 3 then
		for name, entity in pairs(entities) do
			Test.Expect(not entity:IsSceneValid(), name .. " is destroyed")
		end
		Test.Expect(CurrentScene:FindEntity("Survivor"):IsSceneValid(), "Survivor was not destroyed")
		Test.Pass("DestroyParentChild: the flush survived")
	end
end
