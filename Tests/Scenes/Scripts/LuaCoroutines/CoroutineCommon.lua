-- Shared helper: record a result for the reporter. First result for a name wins.
function Report(name, passed, detail)
	_G.CoroutineTests = _G.CoroutineTests or {}
	if _G.CoroutineTests[name] ~= nil then return end
	_G.CoroutineTests[name] = passed and true or (detail or "failed")
end
