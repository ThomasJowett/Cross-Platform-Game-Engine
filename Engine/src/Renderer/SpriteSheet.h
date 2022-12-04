#pragma once

#include "Core/Asset.h"
#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"

#include <filesystem>
#include <unordered_map>

class SpriteSheet : public Asset
{
public:
	SpriteSheet();
	SpriteSheet(const std::filesystem::path& filepath);
	virtual bool Load(const std::filesystem::path& filepath) override;
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime);
	void RemoveAnimation(std::string name);
	void RenameAnimation(const std::string& oldName, const std::string& newName);
	void Animate(float deltaTime);

	void SelectAnimation(const std::string& animationName);

	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }
	void SetSubTexture(Ref<SubTexture2D> subTexture);

	std::unordered_map<std::string, Animation>& GetAnimations() { return m_Animations; }
	const std::string GetCurrentAnimation() const { return m_CurrentAnimation; }

private:
	Ref<SubTexture2D> m_Texture;
	std::unordered_map<std::string, Animation> m_Animations;
	std::string m_CurrentAnimation;
};
