#pragma once

#include "Renderer/SubTexture2D.h"
#include "Animation/Animation.h"
#include "Core/Asset.h"

#include <set>
#include <filesystem>

namespace tinyxml2
{
class XMLDocument;
}

class Tile
{
public:
	enum class CollisionShape
	{
		None,
		Rect,
		Polygon
	};
	void SetProbability(double probability) { m_Probability = probability; }
	double GetProbability() const { return m_Probability; }

	CollisionShape GetCollisionShape() const { return m_CollisionShape; }
	void SetCollisionShape(CollisionShape shape) { m_CollisionShape = shape; }
private:
	double m_Probability = 1.0;
	CollisionShape m_CollisionShape = CollisionShape::None;
	std::vector<Vector2f> m_Vertices;
};

class Tileset : public Asset
{
public:
	enum class Bitmask
	{
		TwoByTwo,
		ThreeByThree
	};

	Tileset();

	Tileset(const std::filesystem::path& filepath);
	Tileset(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;
	bool Save() const;
	bool SaveAs(const std::filesystem::path& filepath) const;

	void SetCurrentTile(uint32_t tile);
	void SetCurrentTile(uint32_t x, uint32_t y);

	Ref<SubTexture2D> GetSubTexture() const { return m_Texture; }
	void SetSubTexture(Ref<SubTexture2D> subTexture);

	void ResizeTiles() { m_Tiles.resize(m_Texture->GetNumberOfCells()); };

	void SetTileProbability(size_t tile, double probability);
	const Tile& GetTile(uint32_t index) { ASSERT(index < m_Tiles.size(), "Index out of range!"); return m_Tiles[index]; }
	const Tile& GetTile(uint32_t x, uint32_t y) const { return m_Tiles[CoordsToIndex(x, y)]; }
	Tile& GetTile(uint32_t x, uint32_t y) { return m_Tiles[CoordsToIndex(x, y)]; }

	const std::set<Tile*>* GetTilesForBitmask(uint32_t bitmask) const;
	int GetBitmaskForTile(const Tile* tile) const;
	void AddBitmask(Bitmask type);
	void SetTileBitmask(Tile* tile, uint16_t bitmask);

	void SetHasCollision(bool hasCollision) { m_HasCollision = hasCollision; };
	bool HasCollision() { return m_HasCollision; }

	size_t GetNumberOfTiles() { return m_Tiles.size(); }

private:
	uint32_t CoordsToIndex(uint32_t x, uint32_t y) const;
	bool LoadXML(tinyxml2::XMLDocument* doc);
	Ref<SubTexture2D> m_Texture;

	std::vector<Tile> m_Tiles;
	std::vector<std::set<Tile*>> m_BitmaskMap;

	bool m_HasCollision = false;
};
