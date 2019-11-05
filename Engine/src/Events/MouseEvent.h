#pragma once

#include "Event.h"

class DLLIMPEXP_CORE MouseMotionEvent : public Event
{
public:
	MouseMotionEvent(float x, float y)
		:m_mouseX(x), m_mouseY(y) {}

	inline float GetX() const { return m_mouseX; }
	inline float GetY() const { return m_mouseY; }

	std::string to_string() const override
	{
		std::stringstream ss; 
		ss << "MouseMotionEvent: " << m_mouseX << ", " << m_mouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::MOUSE_MOTION)
	EVENT_CLASS_CATEGORY(EventCategory::EC_MOUSE | EventCategory::EC_INPUT)
private:
	float m_mouseX, m_mouseY;
};

class DLLIMPEXP_CORE MouseWheelEvent : public Event
{
public:
	MouseWheelEvent(float xOffset, float yOffset)
	:m_XOffset(xOffset), m_YOffset(yOffset) {}

	inline float GetXOffset() const { return m_XOffset; }
	inline float GetYOffset() const { return m_YOffset; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseWheelEvent: " << GetXOffset() << ", " << GetYOffset();
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::MOUSE_WHEEL)
	EVENT_CLASS_CATEGORY(EventCategory::EC_MOUSE | EventCategory::EC_INPUT)

private:
	float m_XOffset, m_YOffset;
};

class DLLIMPEXP_CORE MouseButtonEvent : public Event
{
public:
	inline int GetMouseButton() const { return m_button; }

	EVENT_CLASS_CATEGORY(EventCategory::EC_MOUSE | EventCategory::EC_INPUT)
protected:
	MouseButtonEvent(int button)
		: m_button(button) {}

	int m_button;
};

class DLLIMPEXP_CORE MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(int button)
		:MouseButtonEvent(button) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_PRESSED)
};

class DLLIMPEXP_CORE MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(int button)
		: MouseButtonEvent(button) {}

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::MOUSE_BUTTON_RELEASED)
};