#pragma once

#include "Renderer/SubTexture2D.h"

class Animation
{
public:
	Animation() = default;
	Animation(Ref<SubTexture2D> texture, int startFrame, int frameCount, float holdTime);

	void Start();
	void Update(float deltaTime);

	float GetAnimationLength();

	uint32_t GetStartFrame() { return m_StartFrame; }
	void SetStartFrame(uint32_t startFrame) { m_StartFrame = startFrame; }

	uint32_t GetFrameCount() { return m_FrameCount; }
	void SetFrameCount(uint32_t frameCount) { m_FrameCount = frameCount; }

	float GetFrameTime() { return m_HoldTime; }
	void SetFrameTime(float frameTime) { m_HoldTime = frameTime; }

	void SetTexture(Ref<SubTexture2D> texture) { m_Texture = texture; }

	const std::string& GetName() { return m_Name; }
	void SetName(const std::string& name) { m_Name = name; }
private:
	void Advance();

	uint32_t m_StartFrame = 0, m_FrameCount = 1, m_CurrentFrame = 0;
	float m_HoldTime = 100.0f;
	float m_CurrentFrameTime = 0.0f;

	Ref<SubTexture2D> m_Texture;
	std::string m_Name;

	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(m_StartFrame, m_FrameCount, m_HoldTime);
	}

};