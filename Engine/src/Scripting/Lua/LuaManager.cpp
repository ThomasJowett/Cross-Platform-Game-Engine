#include "stdafx.h"
#include "LuaManager.h"
#include "Core/Application.h"

#include "LuaBindings.h"
#include "sol/sol.hpp"

Scope<sol::state> LuaManager::s_State = nullptr;
static std::vector<std::string> s_Modules;

static sol::function s_UnrequireFunction;

int LoadFileRequire(lua_State* L) {

	// use sol2 stack API to pull
	// "first argument"
	std::string path = sol::stack::get<std::string>(L, 1);

	std::filesystem::path filepath = Application::GetOpenDocumentDirectory();

	if (std::filesystem::exists(filepath / path))
	{
		filepath /= path;
	}
	else if (std::filesystem::exists(filepath / std::string(path + ".lua")))
	{
		filepath /= std::string(path + ".lua");
	}

	if(!filepath.empty())
	{
		luaL_loadfile(L, filepath.string().c_str());
		s_Modules.push_back(path);
		return 1;
	}
	else
	{
		sol::stack::push(
			L, "Make sure to search from the project directory");
		return 1;
	}
}

void UnloadModules()
{
	for (std::string& mod : s_Modules)
	{
		s_UnrequireFunction(mod);
	}
	s_Modules.clear();
}

void LuaManager::Init()
{
	PROFILE_FUNCTION();
	s_State = CreateScope<sol::state>(nullptr);

	s_State->open_libraries(
		sol::lib::base, 
		sol::lib::package, 
		sol::lib::math, 
		sol::lib::table, 
		sol::lib::coroutine, 
		sol::lib::os,
		sol::lib::string);

	Lua::BindLogging(*s_State);
	Lua::BindApp(*s_State);
	Lua::BindScene(*s_State);
	Lua::BindEntity(*s_State);
	Lua::BindInput(*s_State);
	Lua::BindMath(*s_State);
	Lua::BindCommonTypes(*s_State);
	Lua::BindDebug(*s_State);

	const char* lua_function_script = 
	R"(
		function unrequire(m) 
			package.loaded[m] = nil 
			_G[m] = nil 
			Log.Debug("unrequired")
		end)";

	s_State->script(lua_function_script);

	s_UnrequireFunction = (*s_State)["unrequire"];

	s_State->add_package_loader(LoadFileRequire);
}

void LuaManager::Shutdown()
{
	s_UnrequireFunction.abandon();
	CleanUp();
	s_State->clear_package_loaders();
	s_State.reset();
}

void LuaManager::CleanUp()
{
	if (s_State)
	{
		UnloadModules();
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