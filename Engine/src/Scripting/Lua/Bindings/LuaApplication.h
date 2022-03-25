#pragma once
#include "sol/sol.hpp"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"

namespace Lua
{
	namespace App
	{
		void Bind(sol::state& state)
		{
			PROFILE_FUNCTION();
			
			sol::table application = state.create_table("App");

			application.set_function("ShowImGui", [&](sol::this_state s, bool showImGui)
				{
					Application::ShowImGui(showImGui);
				});

			application.set_function("ToggleImGui", [&](sol::this_state s)
				{
					Application::ToggleImGui();
				});

			application.set_function("GetFixedUpdateInterval", [&](sol::this_state s) -> float
				{
					return Application::Get().GetFixedUpdateInterval();
				});
		}
	}
}
