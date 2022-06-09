#pragma once

#include "Renderer/SubTexture2D.h"

#include "cereal/access.hpp"

class Animation
{
public:
	Animation() = default;
	Animation(Ref<SubTexture2D> texture, int startFrame, int frameCount, float holdTime);

	void Start();
	void Update(float deltaTime);

	float GetAnimationLength();

	uint32_t GetStartFrame() const { return m_StartFrame; }
	void SetStartFrame(uint32_t startFrame);

	uint32_t GetFrameCount() const { return m_FrameCount; }
	void SetFrameCount(uint32_t frameCount) { m_FrameCount = frameCount; }

	float GetFrameTime() const { return m_HoldTime; }
	void SetFrameTime(float frameTime) { m_HoldTime = frameTime; }

	void SetTexture(Ref<SubTexture2D> texture) { m_Texture = texture; }

private:
	void Advance();

	uint32_t m_StartFrame = 0, m_FrameCount = 1;
	float m_HoldTime = 0.01f;
	uint32_t m_CurrentFrame = 0;
	float m_CurrentFrameTime = 0.0f;

	Ref<SubTexture2D> m_Texture;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(m_StartFrame, m_FrameCount, m_HoldTime);
	}
};