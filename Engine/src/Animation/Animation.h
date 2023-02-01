#pragma once

#include "Renderer/SubTexture2D.h"

#include "cereal/access.hpp"

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

private:

	uint32_t m_StartFrame = 0;
	uint32_t m_FrameCount = 1;
	float m_HoldTime = 0.01f;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(m_StartFrame, m_FrameCount, m_HoldTime);
	}
};