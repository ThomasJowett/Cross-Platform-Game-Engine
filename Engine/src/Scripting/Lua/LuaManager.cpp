#include "stdafx.h"
#include "LuaManager.h"
#include "Core/Application.h"

#include "LuaBindings.h"
#include "sol/sol.hpp"

Scope<sol::state> LuaManager::s_State = nullptr;

void LuaManager::Init()
{
	s_State = CreateScope<sol::state>(nullptr);

	s_State->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);

	Lua::BindLogging(*s_State);
	Lua::BindApp(*s_State);
	Lua::BindScene(*s_State);
	Lua::BindEntity(*s_State);
	Lua::BindInput(*s_State);
	Lua::BindMath(*s_State);
}

void LuaManager::Shutdown()
{
	CleanUp();
	s_State.reset();
}

void LuaManager::CleanUp()
{
	if(s_State)
	{
		s_State->stack_clear();
		s_State->collect_garbage();
	}
}

sol::state& LuaManager::GetState()
{
	return *s_State;
}

bool LuaManager::IsValid()
{
	return (bool)s_State;
}
