#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"
#include "Core/Settings.h"
#include "Core/Application.h"

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

	SetFunction(application, "App", "ShowImGui", "Show or hide the ImGui overlay", &Application::ShowImGui);
	SetFunction(application, "App", "ToggleImGui", "Toggle the ImGui overlay", &Application::ToggleImGui);

	SetFunction(application, "App", "GetFixedUpdateInterval", "Get the fixed update interval, in seconds", [](sol::this_state s)
		{ return Application::Get().GetFixedUpdateInterval(); });

	SetFunction(application, "App", "MaximizeWindow", "Maximize the application window", [](sol::this_state s)
		{ return Application::GetWindow()->MaximizeWindow(); });
	SetFunction(application, "App", "RestoreWindow", "Restore the application window from maximized/minimized", [](sol::this_state s)
		{ return Application::GetWindow()->RestoreWindow(); });
	SetFunction(application, "App", "SetWindowMode", "Set the window mode (Windowed, Full_Screen or Borderless)", [](sol::this_state s, WindowMode windowMode)
		{ return Application::GetWindow()->SetWindowMode(windowMode); });

	SetFunction(application, "App", "GetDocumentDirectory", "Get the directory of the currently open project", []()
		{ return Application::GetOpenDocumentDirectory().string(); });

	sol::table settings = state.create_table("Settings");
	LuaManager::AddIdentifier("Settings", "Application settings");

	SetFunction(settings, "Settings", "SetValue", "Set a value", &Settings::SetValue);
	SetFunction(settings, "Settings", "SetBool", "Set a boolean", &Settings::SetBool);
	SetFunction(settings, "Settings", "SetDouble", "Set a double", &Settings::SetDouble);
	SetFunction(settings, "Settings", "SetInt", "Set an integer", &Settings::SetInt);
	SetFunction(settings, "Settings", "SetVec2", "Set a vector2", &Settings::SetVector2f);
	SetFunction(settings, "Settings", "SetVec3", "Set a vector3", &Settings::SetVector3f);

	SetFunction(settings, "Settings", "GetValue", "Get a value", &Settings::GetValue);
	SetFunction(settings, "Settings", "GetBool", "Get a boolean", &Settings::GetBool);
	SetFunction(settings, "Settings", "GetDouble", "Get a double", &Settings::GetDouble);
	SetFunction(settings, "Settings", "GetInt", "Get an integer", &Settings::GetInt);
	SetFunction(settings, "Settings", "GetVec2", "Get a vector2", &Settings::GetVector2f);
	SetFunction(settings, "Settings", "GetVec3", "Get a vector3", &Settings::GetVector3f);

	SetFunction(settings, "Settings", "SetDefaultValue", "Set default value", &Settings::SetDefaultValue);
	SetFunction(settings, "Settings", "SetDefaultBool", "Set default boolean", &Settings::SetDefaultBool);
	SetFunction(settings, "Settings", "SetDefaultDouble", "Set default double", &Settings::SetDefaultDouble);
	SetFunction(settings, "Settings", "SetDefaultInt", "Set default integer", &Settings::SetDefaultInt);
	SetFunction(settings, "Settings", "SetDefaultVec2", "Set default vector2", &Settings::SetDefaultVector2f);
	SetFunction(settings, "Settings", "SetDefaultVec3", "Set default vector3", &Settings::SetDefaultVector3f);

	SetFunction(settings, "Settings", "GetDefaultValue", "Get default value", &Settings::GetDefaultValue);
	SetFunction(settings, "Settings", "GetDefaultBool", "Get default boolean", &Settings::GetDefaultBool);
	SetFunction(settings, "Settings", "GetDefaultDouble", "Get default double", &Settings::GetDefaultDouble);
	SetFunction(settings, "Settings", "GetDefaultInt", "Get default integer", &Settings::GetDefaultInt);
	SetFunction(settings, "Settings", "GetDefaultVec2", "Get default vector2", &Settings::GetDefaultVector2f);
	SetFunction(settings, "Settings", "GetDefaultVec3", "Get default vector3", &Settings::GetDefaultVector3f);
}
}