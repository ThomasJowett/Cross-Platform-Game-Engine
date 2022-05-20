﻿#include "stdafx.h"
#include "Tileset.h"

#include "TinyXml2/tinyxml2.h"

Tileset::Tileset(const std::filesystem::path& filepath)
{
	Load(filepath);
}

bool Tileset::Load(const std::filesystem::path& filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		CLIENT_ERROR("Could not load Tileset: {0}, File does not exist!", filepath);
		return false;
	}
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		m_Filepath = filepath;
		tinyxml2::XMLElement* pRoot;

		pRoot = doc.FirstChildElement("tileset");

		const char* name = pRoot->Attribute("name");
		m_Name = name;

		pRoot->QueryUnsignedAttribute("tilewidth", &m_TileWidth);
		pRoot->QueryUnsignedAttribute("tileheight", &m_TileHeight);
		pRoot->QueryUnsignedAttribute("columns", &m_Columns);
		pRoot->QueryUnsignedAttribute("tilecount", &m_TileCount);

		tinyxml2::XMLElement* pImage = pRoot->FirstChildElement("image");

		const char* textureSource = pImage->Attribute("source");

		std::filesystem::path texturePath = filepath;
		texturePath.remove_filename();
		texturePath = texturePath / textureSource;

		m_Texture = CreateRef<SubTexture2D>(Texture2D::Create(texturePath), m_TileWidth, m_TileHeight);

		tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("tile");

		while (pTile)
		{
			Tile tile;
			tile.id = atoi(pTile->Attribute("id"));
			const char* type = pTile->Attribute("type");
			if (type != nullptr)
			{
				tile.type = type;
			}
			const char* probability = pTile->Attribute("probability");
			if (probability != nullptr)
				tile.probability = atof(probability);

			tinyxml2::XMLElement* pAnimation = pTile->FirstChildElement("animation");
			if (pAnimation)
			{
				tinyxml2::XMLElement* pFrame = pAnimation->FirstChildElement("frame");

				while (pFrame)
				{
					// TODO: load in the animation
					pFrame = pFrame->NextSiblingElement("frame");
				}
			}

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

//bool Tileset::Save(const std::filesystem::path& filepath) const
//{
//	//TODO: save tileset back to .tsx format
//	return false;
//}

void Tileset::SetCurrentTile(uint32_t tile)
{
	m_Texture->SetCurrentCell(tile - 1);
	if (m_Animations.find(m_CurrentAnimation) != m_Animations.end())
	{
		m_Animations[m_CurrentAnimation].Start();
	}
}

void Tileset::AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime)
{
	m_Animations.insert({ name, Animation(m_Texture, startFrame, frameCount, holdTime) });
}

void Tileset::Animate(float deltaTime)
{
	if (m_Animations.find(m_CurrentAnimation) != m_Animations.end())
	{
		m_Animations[m_CurrentAnimation].Update(deltaTime);
	}
}

void Tileset::SelectAnimation(const std::string& animationName)
{
	for (auto& [name, animation] : m_Animations)
	{
		if (name == animationName)
		{
			m_Texture->SetCurrentCell(animation.GetStartFrame());
			m_CurrentAnimation = animationName;
			break;
		}
	}
}
