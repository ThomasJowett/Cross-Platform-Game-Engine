#pragma once

#include <string>
#include <sstream>

#include "Core/core.h"

enum class EventType
{
	NONE = 0,

	WINDOW_CLOSE,
	WINDOW_RESIZE,
	WINDOW_FOCUS,
	WINDOW_LOST_FOCUS,
	WINDOW_MOVED,
	WINDOW_MAXIMIZED,

	APP_TICK,
	APP_UPDATE,
	APP_RENDER,
	APP_OPEN_DOCUMENT_CHANGE,

	KEY_PRESSED,
	KEY_RELEASED,
	KEY_TYPED,

	MOUSE_BUTTON_PRESSED,
	MOUSE_BUTTON_RELEASED,
	MOUSE_MOTION,
	MOUSE_WHEEL,

	JOYSTICK_CONNECTED,
	JOYSTICK_DISCONNECTED,

	SCENE_CHANGED,
	SCENE_SAVED,
	SCENE_LOADED,

	FILE_DROP
};

enum class EventCategory
{
	NONE			= 0,
	APPLICATION		= BIT(0),
	INPUT			= BIT(1),
	KEYBOARD		= BIT(2),
	MOUSE			= BIT(3),
	MOUSE_BUTTON	= BIT(4),
	JOYSTICK		= BIT(5),
	SCENE			= BIT(6),
	FILE			= BIT(7)
};

EventCategory operator |(EventCategory lhs, EventCategory rhs);
EventCategory operator & (EventCategory lhs, EventCategory rhs);
EventCategory operator ^(EventCategory lhs, EventCategory rhs);
EventCategory operator ~(EventCategory rhs);
EventCategory operator |=(EventCategory& lhs, EventCategory rhs);
EventCategory operator &=(EventCategory& lhs, EventCategory rhs);
EventCategory operator ^=(EventCategory& lhs, EventCategory rhs);

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategoryFlags() const override { return category; }

class Event
{
public:
	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual EventCategory GetCategoryFlags() const = 0;
	virtual std::string to_string() const { return GetName(); }

	inline bool IsInCategory(EventCategory category)
	{
		return static_cast<bool>(GetCategoryFlags() & category);
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
			m_event.Handled |= func(static_cast<T&>(m_event));
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