#include "stdafx.h"
#include "Tilemap.h"

#include "TinyXml2/tinyxml2.h"

static std::string fileLoadErrorString = "Could not load tilemap {0}. {1}!";

bool Tilemap::Load(std::filesystem::path filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.Parse(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("map");

		const char* orientation = pRoot->Attribute("orientation");

		if (orientation == "orthogonal")
			m_Orientation = Orientation::orthogonal;
		else if (orientation == "isometric")
			m_Orientation = Orientation::isometric;
		else if (orientation == "staggered")
			m_Orientation = Orientation::staggered;
		else if (orientation == "hexagonal")
			m_Orientation = Orientation::hexagonal;

		const char* renderingOrder = pRoot->Attribute("renderorder");

		if (renderingOrder == "right-down")
			m_RenderingOrder = RenderingOrder::rightDown;
		else if (renderingOrder == "right-up")
			m_RenderingOrder = RenderingOrder::rightUp;
		else if (renderingOrder == "left-down")
			m_RenderingOrder = RenderingOrder::leftDown;
		else if (renderingOrder == "left-up")
			m_RenderingOrder = RenderingOrder::leftUp;

		if (m_Orientation == Orientation::staggered || m_Orientation == Orientation::hexagonal)
		{
			const char* staggerAxis = pRoot->Attribute("staggeraxis");

			if (staggerAxis == "Y")
				m_StaggerAxis = StaggerAxis::Y;
			else if (staggerAxis == "X")
				m_StaggerAxis == StaggerAxis::X;
			else
				m_IsStaggered = false;

			const char* staggerIndex = pRoot->Attribute("staggerindex");

			if (staggerIndex == "even")
				m_StaggerIndex = StaggerIndex::even;
			else if (staggerIndex == "odd")
				m_StaggerIndex = StaggerIndex::odd;
			else
				m_IsStaggered = false;
		}
		else
		{
			m_IsStaggered = false;
		}

		const char* backgroundColour = pRoot->Attribute("backgroundcolor");

		if (backgroundColour != NULL)
		{
			m_BackgroundColour.SetColour(backgroundColour);
		}
		else
			m_BackgroundColour = Colour(0.0f, 0.0f, 0.0f, 0.0f);

		m_Width = atoi(pRoot->Attribute("width"));
		m_Hieght = atoi(pRoot->Attribute("height"));
		m_TileWidth = atoi(pRoot->Attribute("tilewidth"));
		m_TileWidth = atoi(pRoot->Attribute("tileheight"));

		tinyxml2::XMLElement* pTileSet = pRoot->FirstChildElement("tilset");

		while (pTileSet)
		{
			const char* tsxPath = pTileSet->Attribute("source");

			Tileset tileset;
			tileset.Load(tsxPath);

			m_Tilesets.push_back(std::make_pair(tileset, atoi(pTileSet->Attribute("source"))));

			pTileSet = pTileSet->NextSiblingElement("tileset");
		}
	}
	else
	{
		CLIENT_ERROR("Could not load tilemap {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	return true;
}

bool Tilemap::Save(std::filesystem::path)
{
	return false;
}

bool Tileset::Load(const std::filesystem::path& filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.Parse(filepath.string().c_str()))
	{
		tinyxml2::XMLElement* pRoot;

		pRoot = doc.FirstChildElement("tileset");

		const char* name = pRoot->Attribute("name");
		m_Name = name;

		m_TileWidth = atoi(pRoot->Attribute("tilewidth"));
		m_TileHeight = atoi(pRoot->Attribute("tileheight"));
		m_Columns = atoi(pRoot->Attribute("columns"));
		m_TileCount = atoi(pRoot->Attribute("tilecount"));

		tinyxml2::XMLElement* pImage = pRoot->FirstChildElement("image");


		const char* textureSource = pRoot->Attribute("source");

		m_Texture = Texture2D::Create(textureSource);

		tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("tile");

		while (pTile)
		{
			pTile = pTile->NextSiblingElement("tile");
		}
	}
	else
	{
		CLIENT_ERROR("Could not load tileset {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	return false;
}