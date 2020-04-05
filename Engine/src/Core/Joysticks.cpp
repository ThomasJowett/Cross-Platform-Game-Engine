#include "stdafx.h"
#include "Joysticks.h"

int Joysticks::s_Joysticks[MAX_JOYSTICKS];
int Joysticks::s_JoystickCount = 0;

double Joysticks::s_DeadZone = 0.1f;

Joysticks::EventCallbackFn Joysticks::s_EventCallback;

void Joysticks::Init()
{
	memset(s_Joysticks, 0, sizeof(s_Joysticks));
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
