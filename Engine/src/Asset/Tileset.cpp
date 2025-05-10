#include "stdafx.h"
#include "Tileset.h"

#include "TinyXml2/tinyxml2.h"
#include "Logging/Instrumentor.h"
#include "Utilities/SerializationUtils.h"
#include "Core/Version.h"

#include <limits>

Tileset::Tileset()
{
	m_Texture = CreateRef<SubTexture2D>();
}

Tileset::Tileset(const std::filesystem::path& filepath)
{
	m_Texture = CreateRef<SubTexture2D>();
	Load(filepath);
}

Tileset::Tileset(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	m_Texture = CreateRef<SubTexture2D>();
	Load(filepath, data);
}

bool Tileset::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);

	if (!std::filesystem::exists(absolutePath))
	{
		ENGINE_ERROR("Could not load Tileset: {0}, File does not exist!", absolutePath);
		return false;
	}
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(absolutePath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load tileset file: {0}", absolutePath);
			return false;
		}
	}
	else
	{
		ENGINE_ERROR("Could not load tileset {0}. {1} on line {2}", absolutePath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	m_Filepath = filepath;
	return true;
}

bool Tileset::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load tileset from memory: {0}", filepath);
			return false;
		}
	}
	m_Filepath = filepath;
	return false;
}

bool Tileset::Save() const
{
	return SaveAs(m_Filepath);
}

bool Tileset::SaveAs(const std::filesystem::path& filepath) const
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("Tileset");

	pRoot->SetAttribute("EngineVersion", VERSION);

	if (m_Texture)
	{
		pRoot->SetAttribute("TileWidth", m_Texture->GetSpriteWidth());
		pRoot->SetAttribute("TileHeight", m_Texture->GetSpriteHeight());
	}

	doc.InsertFirstChild(pRoot);

	if (m_Texture && m_Texture->GetTexture())
	{
		SerializationUtils::Encode(pRoot->InsertNewChildElement("Texture"), m_Texture->GetTexture());
	}

	for (size_t i = 0; i < m_Tiles.size(); i++)
	{
		if (m_Tiles[i].GetProbability() != 1.0 || m_Tiles[i].GetCollisionShape() != Tile::CollisionShape::None)
		{
			tinyxml2::XMLElement* pTile = pRoot->InsertNewChildElement("Tile");
			pTile->SetAttribute("Id", (int64_t)i);
			pTile->SetAttribute("Probability", m_Tiles[i].GetProbability());
			pTile->SetAttribute("Shape", (int)m_Tiles[i].GetCollisionShape());
		}
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

void Tileset::SetCurrentTile(uint32_t tile)
{
	if (m_Texture)
	{
		m_Texture->SetCurrentCell(tile);
	}
}

void Tileset::SetCurrentTile(uint32_t x, uint32_t y)
{
	if (m_Texture)
	{
		m_Texture->SetCurrentCell(CoordsToIndex(x,y));
	}
}

void Tileset::SetTileProbability(size_t tile, double probability)
{
	if (m_Tiles.size() > tile)
		m_Tiles[tile].SetProbability(probability);
}

void Tileset::SetSubTexture(Ref<SubTexture2D> subTexture)
{
	m_Texture = subTexture;
	if (m_Texture && m_Tiles.size() != m_Texture->GetNumberOfCells())
	{
			m_Tiles.resize(m_Texture->GetNumberOfCells());
	}
}

const std::set<Tile*>* Tileset::GetTilesForBitmask(uint32_t bitmask) const
{
	if (bitmask > m_BitmaskMap.size() - 1 || m_BitmaskMap.empty())
		return nullptr;
	return &m_BitmaskMap.at(bitmask);
}

int Tileset::GetBitmaskForTile(const Tile* tile) const
{
	for (size_t i = 0; i < m_BitmaskMap.size(); ++i)
	{
		for (auto& tileBitmask : m_BitmaskMap[i])
		{
			if (tileBitmask == tile)
				return (int)i;
		}
	}
	return 0;
}

void Tileset::AddBitmask(Bitmask type)
{
	if (type == Bitmask::TwoByTwo && m_BitmaskMap.size() != 16)
	{
		m_BitmaskMap.clear();
		m_BitmaskMap.resize(16);
	}
	else if( type == Bitmask::ThreeByThree && m_BitmaskMap.size() != 48)
	{
		m_BitmaskMap.clear();
		m_BitmaskMap.resize(48);
	}
}

void Tileset::SetTileBitmask(Tile* tile, uint16_t bitmask)
{
	for (auto& tiles : m_BitmaskMap)
	{
		for (auto i = tiles.begin(); i != tiles.end();)
		{
			if (*i == tile)
			{
				i = tiles.erase(i);
			}
			else
				++i;
		}
	}
	m_BitmaskMap.at(bitmask).insert(tile);
}

uint32_t Tileset::CoordsToIndex(uint32_t x, uint32_t y) const
{
	if(m_Texture)
		return std::clamp((y * m_Texture->GetCellsWide()) + x, 0U, m_Texture->GetNumberOfCells() - 1);
	return 0;
}

bool Tileset::LoadXML(tinyxml2::XMLDocument* doc)
{
	tinyxml2::XMLElement* pRoot;

	pRoot = doc->FirstChildElement("Tileset");

	if (!pRoot)
	{
		ENGINE_ERROR("Tileset does not have a Tileset node");
		return false;
	}

	if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION) {
		ENGINE_WARN("Tileset created with a different version of the engine");
	}

	m_Tiles.clear();
	Ref<Texture2D> texture;

	SerializationUtils::Decode(pRoot->FirstChildElement("Texture"), texture);

	if (texture)
	{
		uint32_t tileWidth, tileHeight;
		pRoot->QueryUnsignedAttribute("TileWidth", &tileWidth);
		pRoot->QueryUnsignedAttribute("TileHeight", &tileHeight);

		m_Texture = CreateRef<SubTexture2D>(texture, tileWidth, tileHeight);
		m_Tiles.resize(m_Texture->GetNumberOfCells());
	}
	else
		m_Texture = CreateRef<SubTexture2D>();

	tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("Tile");

	while (pTile)
	{
		int tileId = atoi(pTile->Attribute("Id"));
		const char* probability = pTile->Attribute("Probability");
		if (probability != nullptr)
			m_Tiles[tileId].SetProbability(atof(probability));
		const char* shape = pTile->Attribute("Shape");
		if (shape)
		{
			m_Tiles[tileId].SetCollisionShape((Tile::CollisionShape)atoi(shape));
			m_HasCollision = true;
		}

		pTile = pTile->NextSiblingElement("Tile");
	}
	return true;
}
