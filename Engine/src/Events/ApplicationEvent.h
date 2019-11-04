#pragma once

#include "Event.h"

class DLLIMPEXP_CORE WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int width, unsigned int height)
		:m_width(width), m_height(height) {}

	inline unsigned int GetWidth() const { return m_width; }
	inline unsigned int GetHeight() const { return m_height; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_width << ", " << m_height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(EventType::WINDOW_RESIZE)
	EVENT_CLASS_CATEGORY(EventCategory::EC_APPLICATION)


private:
	unsigned int m_width, m_height;
};

class DLLIMPEXP_CORE WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() {}

	EVENT_CLASS_TYPE(EventType::WINDOW_CLOSE)
	EVENT_CLASS_CATEGORY(EventCategory::EC_APPLICATION)
};

class DLLIMPEXP_CORE AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() {}

	EVENT_CLASS_TYPE(EventType::APP_UPDATE)
	EVENT_CLASS_CATEGORY(EventCategory::EC_APPLICATION)
};

class DLLIMPEXP_CORE AppRenderEvent : public Event
{
public:
	AppRenderEvent() {}

	EVENT_CLASS_TYPE(EventType::APP_RENDER)
	EVENT_CLASS_CATEGORY(EventCategory::EC_APPLICATION)
};