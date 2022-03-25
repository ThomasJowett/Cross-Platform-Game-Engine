#include "stdafx.h"
#include "LuaManager.h"

#include "Bindings/LuaLogging.h"
#include "Bindings/LuaApplication.h"
#include "sol/sol.hpp"

sol::state LuaManager::s_State = nullptr;

void LuaManager::Init()
{
	s_State = sol::state(nullptr);

	s_State.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

	Lua::Logging::Bind(s_State);
	Lua::App::Bind(s_State);
}

sol::state& LuaManager::GetState()
{
	return s_State;
}
