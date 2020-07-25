#pragma once

#include "Event.h"

class KeyEvent : public Event
{
public:
	inline int GetKeyCode() const { return m_keyCode; }

	EVENT_CLASS_CATEGORY(EC_KEYBOARD | EC_INPUT)
	
protected:

	KeyEvent(int keyCode)
		:m_keyCode(keyCode) {}
	int m_keyCode;
};

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keycode, int repeatCount)
		:KeyEvent(keycode), m_repeatCount(repeatCount) {}

	inline int GetRepeatCount() const { return m_repeatCount; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KEY_PRESSED)

private:
	int m_repeatCount;
};

class KeyReleasedEvent :public KeyEvent
{
public:
	KeyReleasedEvent(int keyCode)
		:KeyEvent(keyCode) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KEY_RELEASED)
};

class KeyTypedEvent : public KeyEvent
{
public:
	KeyTypedEvent(int keyCode)
		:KeyEvent(keyCode) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KEY_TYPED)
};