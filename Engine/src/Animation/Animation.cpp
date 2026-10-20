#include "Animation.h"

#include <algorithm>

Animation::Animation(int startFrame, int frameCount, float holdTime)
	:m_StartFrame(startFrame), m_FrameCount(frameCount), m_HoldTime(holdTime)
{
}

float Animation::GetAnimationLength() const
{
	return m_HoldTime * (float)m_FrameCount;
}

void Animation::SetStartFrame(uint32_t startFrame)
{
	m_StartFrame = startFrame;
}

bool Animation::AddEvent(uint32_t frame, const std::string& name)
{
	if (name.empty() || HasEvent(frame, name))
		return false;

	auto insertAt = std::upper_bound(m_Events.begin(), m_Events.end(), frame,
		[](uint32_t value, const AnimationEvent& event) { return value < event.frame; });
	m_Events.insert(insertAt, { frame, name });
	return true;
}

bool Animation::RemoveEvent(uint32_t frame, const std::string& name)
{
	auto it = std::find_if(m_Events.begin(), m_Events.end(),
		[&](const AnimationEvent& event) { return event.frame == frame && event.name == name; });
	if (it == m_Events.end())
		return false;
	m_Events.erase(it);
	return true;
}

bool Animation::RenameEvent(uint32_t frame, const std::string& oldName, const std::string& newName)
{
	if (newName.empty() || HasEvent(frame, newName))
		return false;

	auto it = std::find_if(m_Events.begin(), m_Events.end(),
		[&](const AnimationEvent& event) { return event.frame == frame && event.name == oldName; });
	if (it == m_Events.end())
		return false;
	it->name = newName;
	return true;
}

bool Animation::HasEvent(uint32_t frame, const std::string& name) const
{
	return std::any_of(m_Events.begin(), m_Events.end(),
		[&](const AnimationEvent& event) { return event.frame == frame && event.name == name; });
}
