#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"

#include "Core/Input.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Joysticks.h"

#include "Scene/SceneManager.h"
#include <Core/Application.h>

namespace Lua
{
void BindInput(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table input = state.create_table("Input");
	LuaManager::AddIdentifier("Input", "Keyboard, mouse, gamepad and cursor");

	SetFunction(input, "Input", "IsKeyPressed", "Is key pressed", [](char c)
		{
			return Input::IsKeyPressed((int)c);
		});
	SetFunction(input, "Input", "IsMouseButtonPressed", "Is the mouse button pressed", &Input::IsMouseButtonPressed);
	SetFunction(input, "Input", "IsMouseButtonReleased", "Was the mouse button released this frame", &Input::IsMouseButtonReleased);
	SetFunction(input, "Input", "IsMouseJustPressed", "Was the mouse button pressed this frame", &Input::IsMouseJustPressed);
	SetFunction(input, "Input", "GetMousePos", "Get mouse position", &Input::GetMousePos);

	std::initializer_list<std::pair<sol::string_view, int>> mouseItems = {
		{ "Left", MOUSE_BUTTON_LEFT },
		{ "Right", MOUSE_BUTTON_RIGHT },
		{ "Middle", MOUSE_BUTTON_MIDDLE },
	};
	SetEnum(state, "MouseButton", "Mouse buttons, for Input.IsMouseButtonPressed and friends", mouseItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickItems = {
		{ "A", GAMEPAD_BUTTON_A },
		{ "B", GAMEPAD_BUTTON_B },
		{ "X", GAMEPAD_BUTTON_X },
		{ "Y", GAMEPAD_BUTTON_Y },
		{ "LeftBumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "RightBumper", GAMEPAD_BUTTON_RIGHT_BUMPER },
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
	SetEnum(state, "JoystickButton", "Gamepad buttons, for Input.IsJoystickButtonPressed", joystickItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickAxisItems =
	{
		{ "LeftX", GAMEPAD_AXIS_LEFT_X },
		{ "LeftY", GAMEPAD_AXIS_LEFT_Y },
		{ "RightX", GAMEPAD_AXIS_RIGHT_X },
		{ "RightY", GAMEPAD_AXIS_RIGHT_Y },
		{ "LeftTrigger", GAMEPAD_AXIS_LEFT_TRIGGER },
		{ "RightTrigger", GAMEPAD_AXIS_RIGHT_TRIGGER }
	};
	SetEnum(state, "JoystickAxis", "Gamepad sticks and triggers, for Input.GetJoystickAxis", joystickAxisItems);

	SetFunction(input, "Input", "GetJoyStickCount", "Get how many gamepads are connected", &Joysticks::GetJoystickCount);
	SetFunction(input, "Input", "IsJoystickButtonPressed", "IsJoystickButtonPressed(slot, button): whether a gamepad button is held; slots start at 0", &Input::IsJoystickButtonPressed);
	SetFunction(input, "Input", "GetJoystickAxis", "GetJoystickAxis(slot, axis): stick position from -1 to 1, or trigger from 0 to 1 (see JoystickAxis)", &Input::GetJoystickAxis);

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

	SetEnum(state, "Cursors", "Cursor shapes, for Input.SetCursor", cursorItems);

	SetFunction(input, "Input", "SetCursor", "Set the appearance of the cursor", [](sol::this_state s, Cursors cursor)
		{ if (Window* window = Application::GetWindow()) window->SetCursor(cursor); });
	SetFunction(input, "Input", "DisableCursor", "Disable the cursor", [](sol::this_state s)
		{
			Window* window = Application::GetWindow();
			if (window && SceneManager::GetSceneState() == SceneState::Play)
				window->DisableCursor();
		});
	SetFunction(input, "Input", "EnableCursor", "Enable the cursor", [](sol::this_state s)
		{ if (Window* window = Application::GetWindow()) window->EnableCursor(); });
	SetFunction(input, "Input", "SetCursorPosition", "Set the position of the cursor", [](sol::this_state s, double xPos, double yPos)
		{ if (Window* window = Application::GetWindow()) window->SetCursorPosition(xPos, yPos); });
}
}
