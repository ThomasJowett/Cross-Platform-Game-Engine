#pragma once

#include "Events/JoystickEvent.h"

#define MAX_JOYSTICKS 16

class Joysticks
{
public:

	struct Joystick
	{
		int ID = 0;
		int axes = 0;
		int buttons = 0;
		int hats = 0;
	};
	using EventCallbackFn = std::function<void(Event&)>;

	static void AddJoystick(Joystick joystick)
	{
		s_Joysticks[s_JoystickCount++] = joystick;
	}

	static void RemoveJoystick(uint32_t joystickID)
	{
		int i;

		for (i = 0; i < s_JoystickCount; i++)
		{
			if (s_Joysticks[i].ID == joystickID)
				break;
		}

		for (i = i + 1; i < s_JoystickCount; i++)
			s_Joysticks[i - 1] = s_Joysticks[i];

		s_JoystickCount--;
	}

	static int GetJoystickCount() { return s_JoystickCount; }
	static Joystick GetJoystick(int joystickSlot);
	static double GetDeadZone() { return s_DeadZone; }

	static void SetEventCallback(const EventCallbackFn& callback);

	static void CallEvent(JoystickConnected event);
	static void CallEvent(JoystickDisconnected event);

private:
	static Joystick s_Joysticks[MAX_JOYSTICKS];
	static int s_JoystickCount;

	static EventCallbackFn s_EventCallback;

	static double s_DeadZone;
};

/* button codes*/
#define GAMEPAD_BUTTON_A               0
#define GAMEPAD_BUTTON_B               1
#define GAMEPAD_BUTTON_X               2
#define GAMEPAD_BUTTON_Y               3
#define GAMEPAD_BUTTON_LEFT_BUMPER     4
#define GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define GAMEPAD_BUTTON_BACK            6
#define GAMEPAD_BUTTON_START           7
#define GAMEPAD_BUTTON_GUIDE           8
#define GAMEPAD_BUTTON_LEFT_THUMB      9
#define GAMEPAD_BUTTON_RIGHT_THUMB     10
#define GAMEPAD_BUTTON_DPAD_UP         11
#define GAMEPAD_BUTTON_DPAD_RIGHT      12
#define GAMEPAD_BUTTON_DPAD_DOWN       13
#define GAMEPAD_BUTTON_DPAD_LEFT       14
#define GAMEPAD_BUTTON_LAST            GAMEPAD_BUTTON_DPAD_LEFT

#define GAMEPAD_BUTTON_CROSS       GAMEPAD_BUTTON_A
#define GAMEPAD_BUTTON_CIRCLE      GAMEPAD_BUTTON_B
#define GAMEPAD_BUTTON_SQUARE      GAMEPAD_BUTTON_X
#define GAMEPAD_BUTTON_TRIANGLE    GAMEPAD_BUTTON_Y

/* axes codes*/

#define GAMEPAD_AXIS_LEFT_X        0
#define GAMEPAD_AXIS_LEFT_Y        1
#define GAMEPAD_AXIS_RIGHT_X       2
#define GAMEPAD_AXIS_RIGHT_Y       3
#define GAMEPAD_AXIS_LEFT_TRIGGER  4
#define GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define GAMEPAD_AXIS_LAST          GAMEPAD_AXIS_RIGHT_TRIGGER