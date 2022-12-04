#pragma once

#include "Renderer/SubTexture2D.h"

#include "cereal/access.hpp"

class Animation
{
public:
	Animation() = default;
	Animation(int startFrame, int frameCount, float holdTime);

	void Start();
	void Update(float deltaTime, Ref<SubTexture2D> texture);

	float GetAnimationLength() const;

	uint32_t GetStartFrame() const { return m_StartFrame; }
	void SetStartFrame(uint32_t startFrame);

	uint32_t GetFrameCount() const { return m_FrameCount; }
	void SetFrameCount(uint32_t frameCount) { m_FrameCount = frameCount; }

	float GetHoldTime() const { return m_HoldTime; }
	void SetHoldTime(float holdTime) { m_HoldTime = holdTime; }

	uint32_t GetCurrentFame() const { return m_CurrentFrame; }

private:
	void Advance(Ref<SubTexture2D> texture);

	uint32_t m_StartFrame = 0;
	uint32_t m_FrameCount = 1;
	float m_HoldTime = 0.01f;
	uint32_t m_CurrentFrame = 0;
	float m_CurrentFrameTime = 0.0f;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(m_StartFrame, m_FrameCount, m_HoldTime);
	}
};