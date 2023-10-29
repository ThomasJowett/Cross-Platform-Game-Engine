#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"
#include "Core/Settings.h"

namespace Lua
{
void BindApp(sol::state& state)
{
	PROFILE_FUNCTION();

	std::initializer_list<std::pair<sol::string_view, int>> windowModes =
	{
		{ "Windowed", (int)WindowMode::WINDOWED },
		{ "Full_Screen", (int)WindowMode::FULL_SCREEN },
		{ "Borderless", (int)WindowMode::BORDERLESS }
	};
	state.new_enum("WindowMode", windowModes);

	sol::table application = state.create_table("App");
	LuaManager::AddIdentifier("App", "Application");

	application.set_function("ShowImGui", &Application::ShowImGui);
	application.set_function("ToggleImGui", &Application::ToggleImGui);

	application.set_function("GetFixedUpdateInterval", [](sol::this_state s)
		{ return Application::Get().GetFixedUpdateInterval(); });

	application.set_function("MaximizeWindow", [](sol::this_state s)
		{ return Application::GetWindow()->MaximizeWindow(); });
	application.set_function("RestoreWindow", [](sol::this_state s)
		{ return Application::GetWindow()->RestoreWindow(); });
	application.set_function("SetWindowMode", [](sol::this_state s, WindowMode windowMode)
		{ return Application::GetWindow()->SetWindowMode(windowMode); });

	application.set_function("GetDocumentDirectory", []()
		{ return Application::GetOpenDocumentDirectory().string(); });

	sol::table settings = state.create_table("Settings");
	LuaManager::AddIdentifier("Settings", "Application settings");

	SetFunction(settings, "SetValue", "Set a value", &Settings::SetValue);
	SetFunction(settings, "SetBool", "Set a boolean", &Settings::SetBool);
	SetFunction(settings, "SetDouble", "Set a double", &Settings::SetDouble);
	SetFunction(settings, "SetInt", "Set an integer", &Settings::SetInt);
	SetFunction(settings, "SetVec2", "Set a vector2", &Settings::SetVector2f);
	SetFunction(settings, "SetVec3", "Set a vector3", &Settings::SetVector3f);

	SetFunction(settings, "GetValue", "Get a value", &Settings::GetValue);
	SetFunction(settings, "GetBool", "Get a boolean", &Settings::GetBool);
	SetFunction(settings, "GetDouble", "Get a double", &Settings::GetDouble);
	SetFunction(settings, "GetInt", "Get an integer", &Settings::GetInt);
	SetFunction(settings, "GetVec2", "Get a vector2", &Settings::GetVector2f);
	SetFunction(settings, "GetVec3", "Get a vector3", &Settings::GetVector3f);

	SetFunction(settings, "SetDefaultValue", "Set default value", &Settings::SetDefaultValue);
	SetFunction(settings, "SetDefaultBool", "Set default boolean", &Settings::SetDefaultBool);
	SetFunction(settings, "SetDefaultDouble", "Set default double", &Settings::SetDefaultDouble);
	SetFunction(settings, "SetDefaultInt", "Set default integer", &Settings::SetDefaultInt);
	SetFunction(settings, "SetDefaultVec2", "Set default vector2", &Settings::SetDefaultVector2f);
	SetFunction(settings, "SetDefaultVec3", "Set default vector3", &Settings::SetDefaultVector3f);

	SetFunction(settings, "GetDefaultValue", "Get default value", &Settings::GetDefaultValue);
	SetFunction(settings, "GetDefaultBool", "Get default boolean", &Settings::GetDefaultBool);
	SetFunction(settings, "GetDefaultDouble", "Get default double", &Settings::GetDefaultDouble);
	SetFunction(settings, "GetDefaultInt", "Get default integer", &Settings::GetDefaultInt);
	SetFunction(settings, "GetDefaultVec2", "Get default vector2", &Settings::GetDefaultVector2f);
	SetFunction(settings, "GetDefaultVec3", "Get default vector3", &Settings::GetDefaultVector3f);
}
}