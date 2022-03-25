#pragma once

#include "sol/sol.hpp"

class LuaManager
{
public:
	static void Init();

	static sol::state& GetState();

private:
	static sol::state s_State;
};