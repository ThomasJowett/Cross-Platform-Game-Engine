#pragma once

#include "Event.h"

class KeyEvent : public Event
{
public:
	inline int GetKeyCode() const { return m_keyCode; }

	EVENT_CLASS_CATEGORY(EventCategory::KEYBOARD | EventCategory::INPUT)
	
protected:

	KeyEvent(int keyCode)
		:m_keyCode(keyCode) {}
	int m_keyCode;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keycode, bool isRepeat)
		:KeyEvent(keycode), m_IsRepeat(isRepeat) {}

	inline int GetRepeatCount() const { return m_IsRepeat; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (repeat = " << m_IsRepeat << ")";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KEY_PRESSED)

private:
	bool m_IsRepeat;
};

/* ------------------------------------------------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------------------------------------------------ */

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