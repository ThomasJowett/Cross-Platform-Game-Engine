#pragma once

#include "cereal/access.hpp"

#include "Renderer/SpriteSheet.h"
#include "Core/Colour.h"
#include "Scene/AssetManager.h"
#include "Core/Application.h"
#include "Utilities/FileUtils.h"

struct AnimatedSpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<SpriteSheet> spriteSheet;
	std::string animation;
	uint32_t currentFrame = 0;

	AnimatedSpriteComponent() = default;
	AnimatedSpriteComponent(const AnimatedSpriteComponent& other) = default;

	void Animate(float deltaTime);
private:
	float m_CurrentFrameTime = 0.0f;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint);
		std::string relativePath;
		if (spriteSheet && !spriteSheet->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(spriteSheet->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
		archive(animation);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
		{
			spriteSheet = AssetManager::GetAsset<SpriteSheet>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			spriteSheet.reset();
		}
		archive(animation);
		if (spriteSheet && !animation.empty()) {
			Animation* animationRef = spriteSheet->GetAnimation(animation);
			if(animationRef)
				currentFrame = animationRef->GetStartFrame();
		}
	}
};
