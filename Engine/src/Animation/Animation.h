#pragma once

#include "Renderer/SubTexture2D.h"

#include "cereal/access.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include <string>
#include <vector>

struct AnimationEvent
{
	uint32_t frame = 0; // local to this Animation, 0 = its own StartFrame
	std::string name;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(frame, name);
	}
};

class Animation
{
public:
	Animation() = default;
	Animation(int startFrame, int frameCount, float holdTime);

	float GetAnimationLength() const;

	uint32_t GetStartFrame() const { return m_StartFrame; }
	void SetStartFrame(uint32_t startFrame);

	uint32_t GetFrameCount() const { return m_FrameCount; }
	void SetFrameCount(uint32_t frameCount) { m_FrameCount = frameCount; }

	uint32_t GetEndFrame() const { return m_StartFrame + m_FrameCount; }

	float GetHoldTime() const { return m_HoldTime; }
	void SetHoldTime(float holdTime) { m_HoldTime = holdTime; }

	// False if the name is empty or already on that frame
	bool AddEvent(uint32_t frame, const std::string& name);
	bool RemoveEvent(uint32_t frame, const std::string& name);
	// False if the new name is empty or already on that frame
	bool RenameEvent(uint32_t frame, const std::string& oldName, const std::string& newName);
	bool HasEvent(uint32_t frame, const std::string& name) const;
	// Sorted by frame, in the order they were added within a frame
	const std::vector<AnimationEvent>& GetEvents() const { return m_Events; }

private:

	uint32_t m_StartFrame = 0;
	uint32_t m_FrameCount = 1;
	float m_HoldTime = 0.01f;
	std::vector<AnimationEvent> m_Events;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(m_StartFrame, m_FrameCount, m_HoldTime, m_Events);
	}
};
