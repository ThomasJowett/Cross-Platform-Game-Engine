#pragma once

#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"
#include "Core/Asset.h"

#include <map>
#include <filesystem>

class Tile
{
public:
	enum CollisionShape
	{
		None,
		Rect,
		Polygon
	};
	void SetProbability(double probability) { m_Probability = probability; }
	double GetProbability() const { return m_Probability; }

	CollisionShape GetCollisionShape() const { return m_CollisionShape; }
private:
	double m_Probability = 1.0;
	CollisionShape m_CollisionShape = CollisionShape::None;
	std::vector<Vector2f> m_Vertices;
};

class Tileset : public Asset
{
public:
	Tileset();
	Tileset(const std::filesystem::path& filepath);
	virtual bool Load(const std::filesystem::path& filepath) override;
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile);

	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }
	void SetSubTexture(Ref<SubTexture2D> subTexture);

	void SetTileProbability(size_t tile, double probability);
	const Tile& GetTile(uint32_t index) { ASSERT(index < m_Tiles.size(), "Index out of range!"); return m_Tiles[index]; }
private:
	Ref<SubTexture2D> m_Texture;

	std::vector<Tile> m_Tiles;
};
