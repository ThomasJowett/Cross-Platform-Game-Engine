#pragma once

#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"
#include <map>
#include <filesystem>

class Tileset
{
	struct Tile
	{
		int id = -1;
		std::string type;
		double probability = 1.0f;
	};

public:
	Tileset() = default;
	Tileset(const std::filesystem::path& filepath);
	bool Load();
	bool Load(const std::filesystem::path& filepath);
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile);

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }

	void AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime);
	void RemoveAnimation(std::string name);
	void RenameAnimation(const std::string& oldName, const std::string& newName);
	void Animate(float deltaTime);

	void SelectAnimation(const std::string& animationName);

	const std::string& GetName() const { return m_Name; }
	void SetName(std::string name) { m_Name = name; }

	std::map<std::string, Animation>& GetAnimations() { return m_Animations; }
	const std::string GetCurrentAnimation() const { return m_CurrentAnimation; }
private:
	std::string m_Name;
	std::filesystem::path m_Filepath;
	Ref<SubTexture2D> m_Texture;
	uint32_t m_TileCount = 4;
	uint32_t m_Columns = 2;

	std::vector<Tile> m_Tiles;
	std::map<std::string, Animation> m_Animations;
	std::string m_CurrentAnimation;
};
