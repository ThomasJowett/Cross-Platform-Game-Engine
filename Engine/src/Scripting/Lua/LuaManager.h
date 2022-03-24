#pragma once

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

namespace Lua
{
	void Initialize();
	void Shutdown();

	lua_State* GetState();


}