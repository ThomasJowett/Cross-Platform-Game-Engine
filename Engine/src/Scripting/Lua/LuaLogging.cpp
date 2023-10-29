#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Logger.h"
#include "Logging/Instrumentor.h"
#include "LuaManager.h"

namespace Lua
{
void BindLogging(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table log = state.create_table("Log");
	LuaManager::AddIdentifier("Log", "Print to the log");

	SetFunction(log, "Trace", "Print very fine detailed Diagnostic information", [](std::string_view message) { CLIENT_TRACE(message); });
	SetFunction(log, "Info", "Normal application behaviour", [](std::string_view message) { CLIENT_INFO(message); });
	SetFunction(log, "Debug", "Diagnostic information to help the understand the flow of scripts", [](std::string_view message) { CLIENT_DEBUG(message); });
	SetFunction(log, "Warn", "Indicates you may have a problem or unusual situation", [](std::string_view message) { CLIENT_WARN(message); });
	SetFunction(log, "Error", "Serious issue and a failure of something important", [](std::string_view message) { CLIENT_ERROR(message); });
	SetFunction(log, "Critical", "Fatal error only to be called when the application is about to crash", [](std::string_view message) { CLIENT_CRITICAL(message); });
}
}