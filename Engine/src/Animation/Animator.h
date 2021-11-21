#pragma once

#include "Animation.h"
#include <cereal/types/vector.hpp> 

class Animator
{
public:
	void SelectAnimation(uint32_t animation);
	void AddAnimation();
	void Animate(float deltaTime);

	Ref<SubTexture2D> GetTexture() { return m_Texture; }
	std::vector<Animation>& GetAnimations() { return m_Animations; }
private:
	std::vector<Animation> m_Animations;
	Ref<SubTexture2D> m_Texture;

	uint32_t m_CurrentAnimation = 0;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath;
		if (m_Texture && !m_Texture->GetTexture()->GetFilepath().empty())
			relativePath = FileUtils::RelativePath(m_Texture->GetTexture()->GetFilepath(), Application::GetOpenDocumentDirectory()).string();

		archive(cereal::make_nvp("Filepath", relativePath));
		archive(cereal::make_nvp("SpriteHeight", m_Texture->GetSpriteHeight()));
		archive(cereal::make_nvp("SpriteWidth", m_Texture->GetSpriteWidth()));
		archive(m_Animations);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		uint32_t spriteWidth, spriteHeight;
		archive(cereal::make_nvp("Filepath", relativePath));
		archive(cereal::make_nvp("SpriteWidth", spriteWidth));
		archive(cereal::make_nvp("SpriteHeight", spriteHeight));

		if (!relativePath.empty())
		{
			m_Texture = CreateRef<SubTexture2D>(Texture2D::Create(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath)),
				spriteWidth, spriteHeight);
		}

		archive(m_Animations);

		for (Animation& animation : m_Animations)
		{
			animation.SetTexture(m_Texture);
		}
	}
};
