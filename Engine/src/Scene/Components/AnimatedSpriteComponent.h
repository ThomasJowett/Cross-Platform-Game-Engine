#pragma once

#include "cereal/access.hpp"
#include "Renderer/SubTexture2D.h"
#include "Core/Colour.h"
#include "Animation/Animator.h"

struct AnimatedSpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Animator animator;

	AnimatedSpriteComponent() = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(animator);
		archive(tint);
	}
};
