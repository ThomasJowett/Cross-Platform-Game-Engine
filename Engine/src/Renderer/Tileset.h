#pragma once

#include "Renderer/SubTexture2D.h"

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
	bool Load(std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile) { m_Texture->SetCurrentCell(tile - 1); }
	const uint32_t GetTileWidth() const { return m_TileWidth; }
	const uint32_t GetTileHeight() const { return m_TileHeight; }

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
	Ref<SubTexture2D> GetTexture() const { return m_Texture; }
private:
	std::string m_Name;
	std::filesystem::path m_Filepath;
	Ref<SubTexture2D> m_Texture;
	uint32_t m_TileWidth;
	uint32_t m_TileHeight;
	uint32_t m_TileCount;
	uint32_t m_Columns;

	std::vector<Tile> m_Tiles;
};
