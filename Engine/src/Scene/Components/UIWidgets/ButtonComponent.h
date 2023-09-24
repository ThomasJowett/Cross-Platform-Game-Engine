#pragma once

#include "cereal/cereal.hpp"

#include "Renderer/Texture.h"

#include "Widget.h"

struct Button : public Widget
{
	Ref<Texture2D> icon;
	Ref<Texture2D> normalTexture;
	Ref<Texture2D> hoveredTexture;
	Ref<Texture2D> clickedTexture;
	Ref<Texture2D> disabledTexture;



private:
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
			if (animationRef)
				currentFrame = animationRef->GetStartFrame();
		}
	}
};
