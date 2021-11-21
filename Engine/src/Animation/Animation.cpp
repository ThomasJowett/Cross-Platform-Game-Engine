#include "stdafx.h"
#include "Animation.h"

Animation::Animation(Ref<SubTexture2D> texture, int startFrame, int frameCount, float holdTime)
	:m_Texture(texture), m_StartFrame(startFrame), m_FrameCount(frameCount),
	m_HoldTime(holdTime), m_CurrentFrame(startFrame)
{

}

void Animation::Start()
{
	m_CurrentFrame = m_StartFrame;
	m_CurrentFrameTime = 0.0f;
}

void Animation::Update(float deltaTime)
{
	m_CurrentFrameTime += deltaTime;
	while (m_CurrentFrameTime >= m_HoldTime)
	{
		Advance();
		m_CurrentFrameTime -= m_HoldTime;
	}
}

float Animation::GetAnimationLength()
{
	return m_HoldTime * m_FrameCount;
}

void Animation::Advance()
{
	if (++m_CurrentFrame >= m_StartFrame + m_FrameCount)
	{
		m_CurrentFrame = m_StartFrame;

	}

	m_Texture->SetCurrentCell(m_CurrentFrame);
}
