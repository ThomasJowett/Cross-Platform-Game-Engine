-- Records which entity each running copy of this script is attached to.

function OnCreate()
	_G.ChildScriptEntities = _G.ChildScriptEntities or {}
	table.insert(_G.ChildScriptEntities, CurrentEntity:GetID():ToString())
end
