#include "LuaManager.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "LuaBindings.h"
#include "LuaCoroutines.h"
#include "sol/sol.hpp"
#include "Scene/AssetManager.h"

#include <chrono>
#include <fstream>
#include <sstream>

Scope<sol::state> LuaManager::s_State = nullptr;
static std::vector<std::string> s_Modules;
SignalBus LuaManager::s_SignalBus;

static sol::function s_UnrequireFunction;

static bool s_WatchdogActive = false;
static std::chrono::steady_clock::time_point s_WatchdogDeadline;

// Count hook: raising an error here unwinds to the script's protected call, ending a runaway loop
static void WatchdogHook(lua_State* L, lua_Debug*)
{
	if (!s_WatchdogActive || std::chrono::steady_clock::now() < s_WatchdogDeadline)
		return;
	// Fresh budget for the scripts still to run this update
	s_WatchdogDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(LuaManager::WatchdogSeconds);
	luaL_error(L, "script ran for more than %d seconds and was stopped (infinite loop?)", LuaManager::WatchdogSeconds);
}

void LuaManager::StartWatchdog()
{
	s_WatchdogDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(WatchdogSeconds);
	s_WatchdogActive = true;
}

void LuaManager::StopWatchdog()
{
	s_WatchdogActive = false;
}

std::vector<LuaApiEntry> LuaManager::s_Identifiers = {
	{ "CurrentEntity", "Get the entity this script is attached to", LuaApiEntry::Kind::Global, "", "" },
	{ "CurrentScene", "The currently loaded Scene", LuaApiEntry::Kind::Global, "", "" }
};

static constexpr int k_ModuleNotFound = -1;

// Pushes the module's chunk, or an error or "not found" message; returns the load status or k_ModuleNotFound
static int LoadModule(lua_State* L)
{
	PROFILE_FUNCTION();
	const std::string name = sol::stack::get<std::string>(L, 1);
	const std::filesystem::path& projectDirectory = Application::GetOpenDocumentDirectory();

	for (const std::filesystem::path& candidate : { std::filesystem::path(name), std::filesystem::path(name + ".lua") })
	{
		std::string source;
		const std::filesystem::path filepath = projectDirectory / candidate;
		if (std::filesystem::is_regular_file(filepath))
		{
			std::ifstream file(filepath, std::ios::binary);
			std::stringstream buffer;
			buffer << file.rdbuf();
			source = buffer.str();
		}
		else if (std::vector<uint8_t> data; AssetManager::FileExistsInBundle(candidate.generic_string())
			&& AssetManager::GetFileData(candidate.generic_string(), data))
		{
			source.assign(data.begin(), data.end());
		}
		else
		{
			continue;
		}

		const std::string chunkName = "@" + candidate.generic_string();
		int status = luaL_loadbuffer(L, source.data(), source.size(), chunkName.c_str());
		if (status == LUA_OK)
			s_Modules.push_back(name);
		return status;
	}

	sol::stack::push(L, "\n\tno file '" + name + "' in the project folder or asset bundle");
	return k_ModuleNotFound;
}

// package.searchers entry for require("Scripts/Module"), from the project folder or the asset bundle
int LoadFileRequire(lua_State* L)
{
	// No C++ objects here: lua_error longjmps past this frame
	int status = LoadModule(L);
	if (status == LUA_OK || status == k_ModuleNotFound)
		return 1;
	return lua_error(L);
}

void UnloadModules()
{
	PROFILE_FUNCTION();
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
	lua_sethook(s_State->lua_state(), WatchdogHook, LUA_MASKCOUNT, 100000);

	s_State->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::math,
		sol::lib::table,
		sol::lib::coroutine,
		sol::lib::os,
		sol::lib::string,
		sol::lib::io);

	Lua::BindLogging(*s_State);
	Lua::BindApp(*s_State);
	Lua::BindScene(*s_State);
	Lua::BindEntity(*s_State);
	Lua::BindInput(*s_State);
	Lua::BindInputAction(*s_State);
	Lua::BindMath(*s_State);
	Lua::BindCommonTypes(*s_State);
	Lua::BindDebug(*s_State);
	Lua::BindSignaling(*s_State);
	Lua::BindPathfinding(*s_State);
	Lua::BindCoroutines(*s_State);

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
	PROFILE_FUNCTION();
	s_SignalBus.Shutdown();
	s_UnrequireFunction.abandon();
	CleanUp();
	if (s_State) {
		s_State->clear_package_loaders();
		s_State.reset();
	}
}

void LuaManager::CleanUp()
{
	PROFILE_FUNCTION();
	if (s_State)
	{
		s_SignalBus.Shutdown();
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

void LuaManager::AddIdentifier(const std::string& keyword, const std::string& description)
{
	AddApiEntry({ keyword, description, LuaApiEntry::Kind::Global, "", "" });
}

void LuaManager::AddApiEntry(LuaApiEntry entry)
{
	s_Identifiers.push_back(std::move(entry));
}

void RegisterLuaApiEntry(LuaApiEntry entry)
{
	LuaManager::AddApiEntry(std::move(entry));
}
