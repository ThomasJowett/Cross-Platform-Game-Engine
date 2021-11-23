#include "stdafx.h"
#include "Animator.h"

Animator::Animator()
{
	m_SpriteSheet = CreateRef<SubTexture2D>();
}

void Animator::SelectAnimation(uint32_t animation)
{
	CORE_ASSERT(m_Animations.size() >= animation, "Animation not found");

	m_Animations[animation].Start();
	m_CurrentAnimation = animation;
}

void Animator::AddAnimation()
{
	m_Animations.push_back(Animation(m_SpriteSheet, 0, 3, 0.1f));
}

void Animator::Animate(float deltaTime)
{
	if (m_CurrentAnimation < m_Animations.size())
		m_Animations[m_CurrentAnimation].Update(deltaTime);
}
