#include "stdafx.h"
#include "LuaManager.h"

#include "Bindings/LuaLogging.h"

namespace Lua
{
	lua_State* L = nullptr;
	std::filesystem::path s_ScriptPath;

	void Lua::Initialize()
	{
		L = luaL_newstate();
		luaL_openlibs(L);

		Logging::Bind(L);
	}

	void Shutdown()
	{
		lua_close(L);
	}

	lua_State* GetState()
	{
		return L;
	}
}

