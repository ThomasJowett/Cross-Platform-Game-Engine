#include "stdafx.h"
#include "LuaManager.h"

#include "LuaBindings.h"
#include "sol/sol.hpp"

sol::state LuaManager::s_State = nullptr;

void LuaManager::Init()
{
	s_State = sol::state(nullptr);

	s_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

	Lua::BindLogging(s_State);
	Lua::BindApp(s_State);
	Lua::BindScene(s_State);
	Lua::BindEntity(s_State);
	Lua::BindInput(s_State);
}

sol::state& LuaManager::GetState()
{
	return s_State;
}
