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

	static void AddIdentifier(const std::string& keyword, const std::string& description);
	static std::vector<std::pair<std::string, std::string>> GetIdentifiers() { return s_Identifiers; }

private:
	static Scope<sol::state> s_State;

	static std::vector<std::pair<std::string, std::string>> s_Identifiers;
};