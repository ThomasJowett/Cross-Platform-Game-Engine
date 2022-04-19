#pragma once

#include "sol/sol.hpp"

class LuaManager
{
public:
	static void Init();
	static void Shutdown();
	static void CleanUp();

	static sol::state& GetState();

	static bool IsValid();

private:
	static Scope<sol::state> s_State;
};