#pragma once

#include "Core/Asset.h"
#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"

#include <filesystem>
#include <unordered_map>

namespace tinyxml2
{
class XMLDocument;
}

class SpriteSheet : public Asset
{
public:
	SpriteSheet();
	SpriteSheet(const std::filesystem::path& filepath);
	SpriteSheet(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime);
	void RemoveAnimation(std::string name);
	void RenameAnimation(const std::string& oldName, const std::string& newName);

	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }
	void SetSubTexture(Ref<SubTexture2D> subTexture);

	std::unordered_map<std::string, Animation>& GetAnimations() { return m_Animations; }
	Animation* GetAnimation(const std::string& animationName);

private:
	Ref<SubTexture2D> m_Texture;
	std::unordered_map<std::string, Animation> m_Animations;

	bool LoadXML(tinyxml2::XMLDocument* doc);
};
