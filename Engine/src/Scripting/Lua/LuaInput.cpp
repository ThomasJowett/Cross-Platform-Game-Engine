#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"

#include "Core/Input.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Joysticks.h"

namespace Lua
{
void BindInput(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table input = state.create_table("Input");
	LuaManager::AddIdentifier("Input", "Input");

	SetFunction(input, "IsKeyPressed", "Is key pressed", [](char c)
		{
			return Input::IsKeyPressed((int)c);
		});
	SetFunction(input, "IsMouseButtonPressed", "Is the mouse button pressed", &Input::IsMouseButtonPressed);
	SetFunction(input, "GetMousePos", "Get mouse position", &Input::GetMousePos);

	std::initializer_list<std::pair<sol::string_view, int>> mouseItems = {
		{ "Left", MOUSE_BUTTON_LEFT },
		{ "Right", MOUSE_BUTTON_RIGHT },
		{ "Middle", MOUSE_BUTTON_MIDDLE },
	};
	state.new_enum("MouseButton", mouseItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickItems = {
		{ "A", GAMEPAD_BUTTON_A },
		{ "B", GAMEPAD_BUTTON_B },
		{ "X", GAMEPAD_BUTTON_X },
		{ "Y", GAMEPAD_BUTTON_Y },
		{ "LeftBumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "RightBumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "Back", GAMEPAD_BUTTON_BACK },
		{ "Start", GAMEPAD_BUTTON_START },
		{ "Guide",GAMEPAD_BUTTON_GUIDE },
		{ "LeftThumbStick", GAMEPAD_BUTTON_LEFT_THUMB },
		{ "RightThumbStick", GAMEPAD_BUTTON_RIGHT_THUMB },
		{ "Up", GAMEPAD_BUTTON_DPAD_UP },
		{ "Right", GAMEPAD_BUTTON_DPAD_RIGHT },
		{ "Down", GAMEPAD_BUTTON_DPAD_DOWN },
		{ "Left", GAMEPAD_BUTTON_DPAD_LEFT },
		{ "Cross", GAMEPAD_BUTTON_CROSS },
		{ "Circle", GAMEPAD_BUTTON_CIRCLE },
		{ "Square", GAMEPAD_BUTTON_SQUARE },
		{ "Triangle", GAMEPAD_BUTTON_TRIANGLE }
	};
	state.new_enum("JoystickButton", joystickItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickAxisItems =
	{
		{ "LeftX", GAMEPAD_AXIS_LEFT_X },
		{ "LeftY", GAMEPAD_AXIS_LEFT_Y },
		{ "RightX", GAMEPAD_AXIS_RIGHT_X },
		{ "RightY", GAMEPAD_AXIS_RIGHT_Y },
		{ "LeftTrigger", GAMEPAD_AXIS_LEFT_TRIGGER },
		{ "RightTrigger", GAMEPAD_AXIS_RIGHT_TRIGGER }
	};
	state.new_enum("JoystickAxis", joystickAxisItems);

	input.set_function("GetJoyStickCount", &Joysticks::GetJoystickCount);
	input.set_function("IsJoystickButtonPressed", &Input::IsJoystickButtonPressed);
	input.set_function("GetJoystickAxis", &Input::GetJoystickAxis);

	std::initializer_list<std::pair<sol::string_view, int>> cursorItems =
	{
		{ "Arrow", (int)Cursors::Arrow},
		{ "IBeam", (int)Cursors::IBeam},
		{ "CrossHair", (int)Cursors::CrossHair},
		{ "PointingHand", (int)Cursors::PointingHand},
		{ "ResizeEW", (int)Cursors::ResizeEW},
		{ "ResizeNS", (int)Cursors::ResizeNS},
		{ "ResizeNWSE", (int)Cursors::ResizeNWSE},
		{ "ResizeNESW", (int)Cursors::ResizeNESW},
		{ "ResizeAll", (int)Cursors::ResizeAll},
		{ "NotAllowed", (int)Cursors::NotAllowed}
	};

	state.new_enum("Cursors", cursorItems);

	SetFunction(input, "SetCursor", "Set the appearance of the cursor", [](sol::this_state s, Cursors cursor)
		{ return Application::GetWindow()->SetCursor(cursor); });
	SetFunction(input, "DisableCursor", "Disable the cursor", [](sol::this_state s)
		{ return Application::GetWindow()->DisableCursor(); });
	SetFunction(input, "EnableCursor", "Enable the cursor", [](sol::this_state s)
		{ return Application::GetWindow()->EnableCursor(); });
	SetFunction(input, "SetCursorPosition", "Set the position of the cursor", [](sol::this_state s, double xPos, double yPos)
		{ return Application::GetWindow()->SetCursorPosition(xPos, yPos); });
}
}
