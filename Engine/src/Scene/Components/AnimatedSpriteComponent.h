#pragma once

#include "cereal/access.hpp"

#include "Asset/SpriteSheet.h"
#include "Core/Colour.h"

#include "Core/Application.h"
#include "Utilities/FileUtils.h"
#include "Utilities/SerializationUtils.h"
#include "Scripting/Lua/LuaBindings.h"

struct AnimatedSpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<SpriteSheet> spriteSheet;
	std::string animation;
	uint32_t currentFrame = 0;

	AnimatedSpriteComponent() = default;
	AnimatedSpriteComponent(const AnimatedSpriteComponent& other) = default;

	void Animate(float deltaTime);
	REFLECT_LUA_BEGIN(AnimatedSpriteComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Tint", "Colour multiplied over the sprite sheet", "Colour",
			([](Self& c) -> Colour& { return c.tint; }),
			([](Self& c, const Colour& v) { c.tint = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("SpriteSheet", "The sprite sheet the animation frames come from", "SpriteSheet",
			([](Self& c) { return c.spriteSheet; }),
			([](Self& c, const Ref<SpriteSheet>& v) { c.spriteSheet = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Animation", "Name of the sprite sheet animation to play", "string",
			([](Self& c) { return c.animation; }),
			([](Self& c, const std::string& v) { c.animation = v; }))
	REFLECT_LUA_END()

private:
	float m_CurrentFrameTime = 0.0f;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint);
		SerializationUtils::SaveAssetToArchive(archive, spriteSheet);
		archive(animation);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint);
		SerializationUtils::LoadAssetFromArchive(archive, spriteSheet);
		archive(animation);
		if (spriteSheet && !animation.empty()) {
			Animation* animationRef = spriteSheet->GetAnimation(animation);
			if(animationRef)
				currentFrame = animationRef->GetStartFrame();
		}
	}
};
