#pragma once

#include "Event.h"

class JoystickConnected : public Event
{
public:
	JoystickConnected(int jid)
		:m_JoystickID(jid)
	{}

	const int GetJoystickID() const { return m_JoystickID; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Joystick Connected, ID: " << m_JoystickID;
		return ss.str();
	}

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

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Joystick Disconnected, ID: " << m_JoystickID;
		return ss.str();
	}

	EVENT_CLASS_TYPE(JOYSTICK_DISCONNECTED);
	EVENT_CLASS_CATEGORY(EC_JOYSTICK);
private:
	int m_JoystickID;
};