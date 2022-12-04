#include "stdafx.h"
#include "Animation.h"

Animation::Animation(int startFrame, int frameCount, float holdTime)
	:m_StartFrame(startFrame), m_FrameCount(frameCount), m_HoldTime(holdTime),
	m_CurrentFrame(startFrame)
{
}

void Animation::Start()
{
	m_CurrentFrame = m_StartFrame;
	m_CurrentFrameTime = 0.0f;
}

void Animation::Update(float deltaTime, Ref<SubTexture2D> texture)
{
	m_CurrentFrameTime += deltaTime;
	while (m_CurrentFrameTime >= m_HoldTime)
	{
		Advance(texture);
		m_CurrentFrameTime -= m_HoldTime;
	}
}

float Animation::GetAnimationLength() const
{
	return m_HoldTime * (float)m_FrameCount;
}

void Animation::SetStartFrame(uint32_t startFrame)
{
	m_StartFrame = startFrame;
	if (m_CurrentFrame < m_StartFrame)
		m_CurrentFrame = m_StartFrame;
}

void Animation::Advance(Ref<SubTexture2D> texture)
{
	if (++m_CurrentFrame >= m_StartFrame + m_FrameCount)
	{
		m_CurrentFrame = m_StartFrame;
	}

	if(m_CurrentFrame <= texture->GetNumberOfCells())
		texture->SetCurrentCell(m_CurrentFrame);
}