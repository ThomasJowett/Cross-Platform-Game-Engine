#include "stdafx.h"
#include "Animation.h"

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