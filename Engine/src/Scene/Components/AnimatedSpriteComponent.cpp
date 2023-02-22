#include "stdafx.h"
#include "AnimatedSpriteComponent.h"

void AnimatedSpriteComponent::Animate(float deltaTime)
{
	if (!spriteSheet) return;
	if (animation.empty()) {
		currentFrame = 0;
		return;
	}
	Animation* animationRef = spriteSheet->GetAnimation(animation);
	if (!animationRef)
		return;
	if (currentFrame < animationRef->GetStartFrame()
		|| currentFrame >= animationRef->GetEndFrame()) {
		currentFrame = animationRef->GetStartFrame();
	}
	m_CurrentFrameTime += deltaTime;
	while (m_CurrentFrameTime >= animationRef->GetHoldTime() && animationRef->GetHoldTime() > 0.0f)
	{
		if(++currentFrame >= animationRef->GetEndFrame()){
			currentFrame = animationRef->GetStartFrame();
		}
		m_CurrentFrameTime -= animationRef->GetHoldTime();
	}
}
