#pragma once

#include "Event.h"

class DLLIMPEXP_CORE KeyEvent : public Event
{
public:
	inline int GetKeyCode() const { return m_keyCode; }

	EVENT_CLASS_CATEGORY(EventCategory::KEYBOARD | EventCategory::INPUT)
	
protected:

	KeyEvent(int keyCode)
		:m_keyCode(keyCode) {}
	int m_keyCode;
};

class DLLIMPEXP_CORE KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keycode, int repeatCount)
		:KeyEvent(keycode), m_repeatCount(repeatCount) {}

	inline int GetRepeatCount() const { return m_repeatCount; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats";
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KEY_PRESSED)

private:
	int m_repeatCount;
};

class DLLIMPEXP_CORE KeyReleasedEvent :public KeyEvent
{
	KeyReleasedEvent(int keyCode)
		:KeyEvent(keyCode) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KEY_RELEASED)
};

class DLLIMPEXP_CORE KeyTypedEvent : public KeyEvent
{
	KeyTypedEvent(int keyCode)
		:KeyEvent(keyCode) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::KEY_TYPED)
};