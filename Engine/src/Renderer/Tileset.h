#pragma once

#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"
#include "Asset.h"

#include <map>
#include <filesystem>

class Tileset : public Asset
{
public:
	Tileset();
	Tileset(const std::filesystem::path& filepath);
	virtual bool Load(const std::filesystem::path& filepath) override;
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile);

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }
	void SetSubTexture(Ref<SubTexture2D> subTexture);

	void AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime);
	void RemoveAnimation(std::string name);
	void RenameAnimation(const std::string& oldName, const std::string& newName);
	void Animate(float deltaTime);

	void SelectAnimation(const std::string& animationName);

	std::map<std::string, Animation>& GetAnimations() { return m_Animations; }
	const std::string GetCurrentAnimation() const { return m_CurrentAnimation; }

	void SetTileProbability(size_t tile, double probability);
private:
	Ref<SubTexture2D> m_Texture;

	std::vector<double> m_TileProbabilities;
	std::map<std::string, Animation> m_Animations;
	std::string m_CurrentAnimation;
};
