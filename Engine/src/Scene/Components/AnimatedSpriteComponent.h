#pragma once

#include "cereal/access.hpp"
#include "Renderer/SubTexture2D.h"
#include "Renderer/Tileset.h"
#include "Core/Colour.h"
#include "Animation/Animator.h"

struct AnimatedSpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<Tileset> tileset;

	AnimatedSpriteComponent() = default;

	void SelectAnimation(const std::string& animationName)
	{
		if (tileset)
			tileset->SelectAnimation(animationName);
		m_CurrentAnimation = animationName;
	}

	const std::string GetCurrentAnimationName() const { return m_CurrentAnimation; }
private:
	std::string m_CurrentAnimation;
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(tint);
		std::string relativePath;
		if (tileset && !tileset->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(tileset->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(cereal::make_nvp("Filepath", relativePath));
		archive(cereal::make_nvp("Current Animation", m_CurrentAnimation));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(tint);
		std::string relativePath;
		archive(cereal::make_nvp("Filepath", relativePath));
		if (!relativePath.empty())
		{
			tileset = AssetManager::GetTileset(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			tileset = nullptr;
		}
		archive(cereal::make_nvp("Current Animation", m_CurrentAnimation));
		if (tileset)
			tileset->SelectAnimation(m_CurrentAnimation);
	}
};
