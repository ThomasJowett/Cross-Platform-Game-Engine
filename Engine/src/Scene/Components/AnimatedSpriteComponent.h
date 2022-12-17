#pragma once

#include "cereal/access.hpp"

#include "Renderer/SpriteSheet.h"
#include "Core/Colour.h"
#include "Scene/AssetManager.h"
#include "Core/Application.h"

struct AnimatedSpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<SpriteSheet> spriteSheet;

	AnimatedSpriteComponent() = default;

	void SelectAnimation(const std::string& animationName)
	{
		if (spriteSheet)
			spriteSheet->SelectAnimation(animationName);
		m_CurrentAnimation = animationName;
	}

	std::string GetCurrentAnimationName() const { return m_CurrentAnimation; }
private:
	std::string m_CurrentAnimation;
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint);
		std::string relativePath;
		if (spriteSheet && !spriteSheet->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(spriteSheet->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
		archive(m_CurrentAnimation);
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
		archive(m_CurrentAnimation);
		if (spriteSheet)
			spriteSheet->SelectAnimation(m_CurrentAnimation);
	}
};
