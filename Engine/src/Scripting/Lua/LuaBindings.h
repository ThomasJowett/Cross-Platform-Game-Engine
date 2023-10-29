#pragma once
#include "sol/sol.hpp"
#include "LuaManager.h"

namespace Lua
{
void BindLogging(sol::state& state);
void BindApp(sol::state& state);
void BindScene(sol::state& state);
void BindEntity(sol::state& state);
void BindInput(sol::state& state);
void BindMath(sol::state& state);
void BindCommonTypes(sol::state& state);
void BindDebug(sol::state& state);

template<typename T, typename... Args>
void SetFunction(T& type, const std::string& name, const std::string& description, Args&&... args)
{
	type.set_function(name, std::forward<Args>(args)...);
	LuaManager::AddIdentifier(name, description);
}
}
