#pragma once

#include "Renderer/SubTexture2D.h"

class Tileset
{
public:
	bool Load(std::filesystem::path& filepath);
	bool Save(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile) { m_Texture->SetCurrentCell(tile); }
	const uint32_t GetTileWidth() const { return m_TileWidth; }
	const uint32_t GetTileHeight() const { return m_TileHeight; }

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
private:
	std::filesystem::path m_Filepath;
	Ref<SubTexture2D> m_Texture;
	uint32_t m_TileWidth;
	uint32_t m_TileHeight;
	uint32_t m_TileCount;
	uint32_t m_Columns;
};
