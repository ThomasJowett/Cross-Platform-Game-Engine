#pragma once

#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"

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
	Tileset(std::filesystem::path& filepath);
	bool Load(std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile);
	const uint32_t GetTileWidth() const { return m_TileWidth; }
	const uint32_t GetTileHeight() const { return m_TileHeight; }

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }

	void AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime);
	void Animate(float deltaTime);

	void SelectAnimation(const std::string& animationName);
private:
	std::string m_Name;
	std::filesystem::path m_Filepath;
	Ref<SubTexture2D> m_Texture;
	uint32_t m_TileWidth;
	uint32_t m_TileHeight;
	uint32_t m_TileCount;
	uint32_t m_Columns;

	std::vector<Tile> m_Tiles;
	std::unordered_map<std::string, Animation> m_Animations;
	std::string m_CurrentAnimation;
};
