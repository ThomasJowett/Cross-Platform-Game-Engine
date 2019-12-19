#pragma once

#include "stdafx.h"
#include "Core/core.h"


enum class EventType
{
	NONE = 0,

	WINDOW_CLOSE,
	WINDOW_RESIZE,
	WINDOW_FOCUS,
	WINDOW_LOST_FOCUS,
	WINDOW_MOVED,

	APP_TICK,
	APP_UPDATE,
	APP_RENDER,

	KEY_PRESSED,
	KEY_RELEASED,
	KEY_TYPED,

	MOUSE_BUTTON_PRESSED,
	MOUSE_BUTTON_RELEASED,
	MOUSE_MOTION,
	MOUSE_WHEEL
};

enum EventCategory
{
	EC_NONE			= 0,
	EC_APPLICATION	= BIT(0),
	EC_INPUT		= BIT(1),
	EC_KEYBOARD		= BIT(2),
	EC_MOUSE		= BIT(3),
	EC_MOUSE_BUTTON	= BIT(4),
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class Event
{
	friend class Event;
public:
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string to_string() const { return GetName(); }

	inline bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & category;
	}

public:
	bool Handled = false;
};

class EventDispatcher
{
public:
	EventDispatcher(Event& event)
		:m_event(event)
	{
	}

	template<typename T, typename F>
	bool Dispatch(const F& func)
	{
		if (m_event.GetEventType() == T::GetStaticType())
		{
			m_event.Handled = func(static_cast<T&>(m_event));
			return true;
		}
		return false;
	}
private:
	Event& m_event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.to_string();
}