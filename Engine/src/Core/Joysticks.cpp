#include "stdafx.h"
#include "Joysticks.h"

Joysticks::Joystick Joysticks::s_Joysticks[MAX_JOYSTICKS];
int Joysticks::s_JoystickCount = 0;

double Joysticks::s_DeadZone = 0.1f;

Joysticks::EventCallbackFn Joysticks::s_EventCallback;

Joysticks::Joystick Joysticks::GetJoystick(int joystickSlot)
{
	if (joystickSlot < s_JoystickCount && joystickSlot >= 0)
	{
		return s_Joysticks[joystickSlot];
	}
	return Joystick();
}

void Joysticks::SetEventCallback(const EventCallbackFn& callback)
{
	s_EventCallback = callback;
}

void Joysticks::CallEvent(JoystickConnected event)
{
	s_EventCallback(event);
}

void Joysticks::CallEvent(JoystickDisconnected event)
{
	s_EventCallback(event);
}
