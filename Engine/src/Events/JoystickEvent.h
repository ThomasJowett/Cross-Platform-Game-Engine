#pragma once

#include "Event.h"

class JoystickConnected : public Event
{
public:
	JoystickConnected(int jid)
		:m_JoystickID(jid)
	{}

	EVENT_CLASS_TYPE(JOYSTICK_CONNECTED);
	EVENT_CLASS_CATEGORY(EC_JOYSTICK);
private:
	int m_JoystickID;
};

class JoystickDisconnected :public Event
{
public:
	JoystickDisconnected(int jid)
		:m_JoystickID(jid)
	{}

	EVENT_CLASS_TYPE(JOYSTICK_DISCONNECTED);
	EVENT_CLASS_CATEGORY(EC_JOYSTICK);
private:
	int m_JoystickID;
};