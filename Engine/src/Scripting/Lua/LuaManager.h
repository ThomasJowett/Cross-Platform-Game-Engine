#pragma once
#define SOL_NO_STD_OPTIONAL 1
#include "sol/sol.hpp"

#include "Core/core.h"
#include "Signaling.hpp"
#include "LuaApiEntry.h"

class LuaManager
{
public:
	static void Init();
	static void Shutdown();
	static void CleanUp();

	static sol::state& GetState();

	static bool IsValid();

	static void AddIdentifier(const std::string& keyword, const std::string& description);
	static void AddApiEntry(LuaApiEntry entry);
	static const std::vector<LuaApiEntry>& GetIdentifiers() { return s_Identifiers; }

	static SignalBus& GetSignalBus() { return s_SignalBus; }

	// While running, a script that takes longer than WatchdogSeconds is stopped with a Lua error
	static constexpr int WatchdogSeconds = 5;
	static void StartWatchdog();
	static void StopWatchdog();

private:
	static Scope<sol::state> s_State;

	static std::vector<LuaApiEntry> s_Identifiers;

	static SignalBus s_SignalBus;
};