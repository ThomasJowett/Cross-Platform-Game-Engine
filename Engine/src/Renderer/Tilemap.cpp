#include "stdafx.h"
#include "Tilemap.h"

#include "TinyXml2/tinyxml2.h"

#include "Utilities/StringUtils.h"

static std::string fileLoadErrorString = "Could not load tilemap {0}. {1}!";

bool Tilemap::Load(std::filesystem::path filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		m_Filepath = filepath;

		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("map");

		const char* orientationchar = pRoot->Attribute("orientation");

		if (orientationchar != nullptr)
		{
			std::string orientation(orientationchar);

			if (orientation == "orthogonal")
				m_Orientation = Orientation::orthogonal;
			else if (orientation == "isometric")
				m_Orientation = Orientation::isometric;
			else if (orientation == "staggered")
				m_Orientation = Orientation::staggered;
			else if (orientation == "hexagonal")
				m_Orientation = Orientation::hexagonal;
		}

		const char* renderingOrderChar = pRoot->Attribute("renderorder");

		if (renderingOrderChar != nullptr)
		{
			std::string renderingOrder(renderingOrderChar);

			if (renderingOrder == "right-down")
				m_RenderingOrder = RenderingOrder::rightDown;
			else if (renderingOrder == "right-up")
				m_RenderingOrder = RenderingOrder::rightUp;
			else if (renderingOrder == "left-down")
				m_RenderingOrder = RenderingOrder::leftDown;
			else if (renderingOrder == "left-up")
				m_RenderingOrder = RenderingOrder::leftUp;
		}

		if (m_Orientation == Orientation::staggered || m_Orientation == Orientation::hexagonal)
		{
			const char* staggerAxisChar = pRoot->Attribute("staggeraxis");

			std::string staggerAxis(staggerAxisChar);

			if (staggerAxis == "y")
				m_StaggerAxis = StaggerAxis::Y;
			else if (staggerAxis == "x")
				m_StaggerAxis = StaggerAxis::X;
			else
				m_IsStaggered = false;

			const char* staggerIndexChar = pRoot->Attribute("staggerindex");

			std::string staggerIndex(staggerIndexChar);

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
		m_Height = atoi(pRoot->Attribute("height"));
		m_TileWidth = atoi(pRoot->Attribute("tilewidth"));
		m_TileHeight = atoi(pRoot->Attribute("tileheight"));

		m_Infinite = atoi(pRoot->Attribute("infinite"));

		tinyxml2::XMLElement* pTileSet = pRoot->FirstChildElement("tileset");

		std::filesystem::path fileDirectory = filepath;
		fileDirectory.remove_filename();

		while (pTileSet)
		{
			const char* tsxPath = pTileSet->Attribute("source");

			std::filesystem::path tilesetPath(fileDirectory / tsxPath);

			Tileset tileset;
			tileset.Load(tilesetPath);

			m_Tilesets.push_back(std::make_pair(tileset, atoi(pTileSet->Attribute("firstgid"))));

			pTileSet = pTileSet->NextSiblingElement("tileset");
		}

		// Layers ------------------------------------------------------------------------------------------------------
		tinyxml2::XMLElement* pLayer = pRoot->FirstChildElement("layer");

		while (pLayer)
		{
			uint32_t id = atoi(pLayer->Attribute("id"));
			std::string name = pLayer->Attribute("name");
			uint32_t width = atoi(pLayer->Attribute("width"));
			uint32_t height = atoi(pLayer->Attribute("height"));

			const char* verticalOffsetChar = pLayer->Attribute("offsetx");
			const char* horizontalOffsetChar = pLayer->Attribute("offsety");

			Vector2f offset(
				verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) : 0.0f,
				horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) : 0.0f
			);

			Layer layer(id, name, width, height, offset);

			tinyxml2::XMLElement* pData = pLayer->FirstChildElement("data");

			const char* encoding = pData->Attribute("encoding");

			if (!strcmp(encoding, "csv"))
			{
				if (layer.ParseCsv(pData->GetText()))
				{
					ENGINE_ERROR("Could not parse tilemap layer {0}", name);
					return false;
				}
			}
			else if (!strcmp(encoding, "base64"))
			{
				ENGINE_ERROR("Could not load tilemap. {0} encoding not yet supported", encoding);
				return false;
			}
			else
			{
				ENGINE_ERROR("Could not load tilemap. {0} encoding not recognised", encoding);
				return false;
			}

			m_Layers.push_back(layer);
			pLayer = pRoot->NextSiblingElement("layer");
		}

		// object groups ------------------------------------------------------------------------------------------------------
		tinyxml2::XMLElement* pObjectGroup = pRoot->FirstChildElement("objectgroup");

		while (pObjectGroup)
		{


			pObjectGroup = pRoot->NextSiblingElement("objectgroup");
		}
	}
	else
	{
		CLIENT_ERROR("Could not load tilemap {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	return true;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Tilemap::Save(std::filesystem::path filepath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("map");

	switch (m_Orientation)
	{
	case Tilemap::Orientation::orthogonal:
		pRoot->SetAttribute("orientation", "orthogonal");
		break;
	case Tilemap::Orientation::isometric:
		pRoot->SetAttribute("orientation", "isometric");
		break;
	case Tilemap::Orientation::staggered:
		pRoot->SetAttribute("orientation", "staggered");
		break;
	case Tilemap::Orientation::hexagonal:
		pRoot->SetAttribute("orientation", "hexagonal");
		break;
	default:
		break;
	}

	
	doc.InsertFirstChild(pRoot);
	//doc.InsertEndChild(pRoot);

	FILE* fp = fopen(filepath.string().c_str(), "wb");
	tinyxml2::XMLError error =  doc.SaveFile(fp);
	return error == tinyxml2::XML_SUCCESS;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool Tileset::Load(std::filesystem::path& filepath)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
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


		const char* textureSource = pImage->Attribute("source");

		std::filesystem::path texturepath = filepath.remove_filename() / textureSource;
		m_Texture = Texture2D::Create(texturepath);

		tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("tile");

		while (pTile)
		{
			Tile tile;
			tile.Id = atoi(pTile->Attribute("id"));
			tile.Type = pTile->Attribute("type");
			const char* probability = pTile->Attribute("probability");
			if (probability != nullptr)
				tile.Probability = atof(probability);

			m_Tiles.push_back(tile);
			pTile = pTile->NextSiblingElement("tile");
		}
	}
	else
	{
		CLIENT_ERROR("Could not load tileset {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	return true;
}

Tilemap::Layer::Layer(uint32_t id, const std::string& name, uint32_t width, uint32_t height, Vector2f offset)
	:m_Id(id), m_Name(name),
	m_Width(width), m_Height(height),
	m_Offset(offset)
{
	m_Tiles = new uint32_t * [m_Height];

	for (uint32_t i = 0; i < m_Height; i++)
	{
		m_Tiles[i] = new uint32_t[m_Width];
	}
}

bool Tilemap::Layer::ParseCsv(const std::string& data)
{
	std::vector<std::string> SeperatedData = SplitString(data, ',');

	ASSERT((uint32_t)SeperatedData.size() == m_Width * m_Height, "Data not the correct length");

	for (uint32_t i = 0; i < m_Height; i++)
	{
		for (uint32_t j = 0; j < m_Width; j++)
		{
			int index = atoi(SeperatedData[(i * m_Width) + j].c_str());
			if (index == 0)
				return false;
			m_Tiles[j][i] = index - 1;
		}
	}
	return true;
}
