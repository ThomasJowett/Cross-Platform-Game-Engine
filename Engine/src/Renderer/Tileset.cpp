#include "stdafx.h"
#include "Tileset.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Colour.h"
#include "Logging/Instrumentor.h"
#include "Utilities/SerializationUtils.h"

#include <limits>

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

		m_Animations.clear();
		m_Tiles.clear();
		tinyxml2::XMLElement* pRoot;

		pRoot = doc.FirstChildElement("tileset");

		const char* name = pRoot->Attribute("name");
		m_Name = name;

		uint32_t tileWidth, tileHeight;

		pRoot->QueryUnsignedAttribute("tilewidth", &tileWidth);
		pRoot->QueryUnsignedAttribute("tileheight", &tileHeight);
		pRoot->QueryUnsignedAttribute("columns", &m_Columns);
		pRoot->QueryUnsignedAttribute("tilecount", &m_TileCount);

		tinyxml2::XMLElement* pImage = pRoot->FirstChildElement("image");

		const char* textureSource = pImage->Attribute("source");

		if (textureSource)
		{
			std::filesystem::path texturePath = filepath;
			texturePath.remove_filename();
			texturePath = texturePath / textureSource;
			m_Texture = CreateRef<SubTexture2D>(Texture2D::Create(texturePath), tileWidth, tileHeight);
		}
		else
		{
			m_Texture = CreateRef<SubTexture2D>(nullptr, tileWidth, tileHeight);
		}

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

				uint32_t startFrame = std::numeric_limits<uint32_t>::max();
				uint32_t endFrame = 0;

				float duration = 0.0f;

				while (pFrame)
				{
					const char* tileid = pFrame->Attribute("tileid");
					if ((uint32_t)atoi(tileid) < startFrame)
						startFrame = atoi(tileid);
					if ((uint32_t)atoi(tileid) > endFrame)
						endFrame = atoi(tileid);
					pFrame->QueryFloatAttribute("duration", &duration);
					pFrame = pFrame->NextSiblingElement("frame");
				}
				AddAnimation("Unnamed Animation", startFrame, endFrame - startFrame, duration);
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

bool Tileset::Save() const
{
	return SaveAs(m_Filepath);
}

bool Tileset::SaveAs(const std::filesystem::path& filepath) const
{
	//TODO: save tileset back to .tsx format

	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("tileset");

	pRoot->SetAttribute("name", m_Name.c_str());
	pRoot->SetAttribute("tilewidth", m_Texture ? m_Texture->GetSpriteWidth() : 16);
	pRoot->SetAttribute("tileheight", m_Texture ? m_Texture->GetSpriteHeight() : 16);
	pRoot->SetAttribute("tilecount", m_TileCount);
	pRoot->SetAttribute("columns", m_Columns);
	pRoot->SetAttribute("backgroundcolor", Colour().HexCode().c_str());

	doc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement* pImage = pRoot->InsertNewChildElement("image");

	if (m_Texture)
	{
		std::string texturePath = FileUtils::RelativePath(m_Texture->GetTexture()->GetFilepath(), m_Filepath).string();
		std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
		pImage->SetAttribute("source", texturePath.c_str());
		pImage->SetAttribute("width", m_Texture->GetTexture()->GetWidth());
		pImage->SetAttribute("height", m_Texture->GetTexture()->GetHeight());
	}

	for (auto& [name, animation] : m_Animations)
	{
		tinyxml2::XMLElement* pTile = pRoot->InsertNewChildElement("tile");

		pTile->SetAttribute("id", animation.GetStartFrame());
		tinyxml2::XMLElement* pAnimation = pTile->InsertNewChildElement("animation");
		for (size_t i = 0; i < animation.GetFrameCount(); i++)
		{
			tinyxml2::XMLElement* pFrame = pAnimation->InsertNewChildElement("frame");
			pFrame->SetAttribute("tileid", animation.GetStartFrame() + i);
			pFrame->SetAttribute("duration", animation.GetFrameTime());
		}
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

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
	if (m_Animations.find(name) != m_Animations.end())
	{
		uint32_t index = 1;
		while (m_Animations.find(name + " (" + std::to_string(index) + ")") != m_Animations.end())
		{
			index++;
		}
		name = name + " (" + std::to_string(index) + ")";
	}
	m_Animations.insert({ name, Animation(m_Texture, startFrame, frameCount, holdTime) });
}

void Tileset::RemoveAnimation(std::string name)
{
	m_Animations.erase(name);
}

void Tileset::RenameAnimation(const std::string& oldName, const std::string& newName)
{
	if (m_Animations.find(newName) == m_Animations.end())
	{
		auto node = m_Animations.extract(oldName);
		if (!node.empty())
		{
			node.key() = newName;
			m_Animations.insert(std::move(node));
		}
	}
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
	if (m_Animations.find(animationName) != m_Animations.end())
	{
		m_Texture->SetCurrentCell(m_Animations.at(animationName).GetStartFrame());
		m_CurrentAnimation = animationName;
	}
}
