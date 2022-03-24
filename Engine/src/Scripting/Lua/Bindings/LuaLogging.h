#pragma once

#include "lua/lua.h"

#include "Logging/Logger.h"

namespace Lua
{
	namespace Logging
	{
		void Bind(lua_State* L)
		{
			lua_register(L, "Log", lua_Log);
		}

		int lua_Log(lua_State* L)
		{
			const char* message = lua_tostring(L, 1);
			CLIENT_DEBUG(message);
			return 0;
		}
	}
}