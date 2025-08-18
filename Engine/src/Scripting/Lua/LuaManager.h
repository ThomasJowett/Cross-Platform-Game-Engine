#pragma once
#define SOL_NO_STD_OPTIONAL 1
#include "sol/sol.hpp"

#include "Core/core.h"
#include "Signaling.hpp"

class LuaManager
{
public:
	static void Init();
	static void Shutdown();
	static void CleanUp();

	static sol::state& GetState();

	static bool IsValid();

	static void AddIdentifier(const std::string& keyword, const std::string& description);
	static const std::vector<std::pair<std::string, std::string>>& GetIdentifiers() { return s_Identifiers; }

	static SignalBus& GetSignalBus() { return s_SignalBus; }

private:
	static Scope<sol::state> s_State;

	static std::vector<std::pair<std::string, std::string>> s_Identifiers;

	static SignalBus s_SignalBus;
};