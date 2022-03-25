#pragma once

#include "lua/lua.h"
#include "sol/sol.hpp"

#include "Logging/Logger.h"

namespace Lua
{
	namespace Logging
	{
		void Bind(sol::state& state)
		{
			PROFILE_FUNCTION();

			sol::table log = state.create_table("Log");

			log.set_function("Trace", [&](sol::this_state s, std::string_view message)
				{
					CLIENT_TRACE(message);
				});

			log.set_function("Info", [&](sol::this_state s, std::string_view message)
				{
					CLIENT_INFO(message);
				});

			log.set_function("Debug", [&](sol::this_state s, std::string_view message)
				{
					CLIENT_DEBUG(message);
				});

			log.set_function("Error", [&](sol::this_state s, std::string_view message)
				{
					CLIENT_ERROR(message);
				});

			log.set_function("Critical", [&](sol::this_state s, std::string_view message)
				{
					CLIENT_CRITICAL(message);
				});
		}
	}
}