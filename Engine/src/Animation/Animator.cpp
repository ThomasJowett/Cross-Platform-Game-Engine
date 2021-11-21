#include "stdafx.h"
#include "Animator.h"

void Animator::SelectAnimation(uint32_t animation)
{
	CORE_ASSERT(m_Animations.size() >= animation, "Animation not found");
	
	m_Animations[animation].Start();
	m_CurrentAnimation = animation;
}

void Animator::AddAnimation()
{
	m_Animations.push_back(Animation(m_Texture, 0, 3, 100.0f));
}

void Animator::Animate(float deltaTime)
{
	m_Animations[m_CurrentAnimation].Update(deltaTime);
}
