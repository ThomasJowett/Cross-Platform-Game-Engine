#include "stdafx.h"
#include "Tileset.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Colour.h"
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
		tinyxml2::XMLElement* pRoot;

		pRoot = doc.FirstChildElement("Tileset");

		if (!pRoot)
		{
			CLIENT_ERROR("Tileset does not have a Tileset node");
			return false;
		}

		if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION) {
			ENGINE_WARN("Tileset created with a different version of the engine");
		}

		Ref<Texture2D> texture;

		SerializationUtils::Decode(pRoot->FirstChildElement("Texture"), texture);

		m_TileProbabilities.clear();
		if (texture)
		{
			uint32_t tileWidth, tileHeight;
			pRoot->QueryUnsignedAttribute("TileWidth", &tileWidth);
			pRoot->QueryUnsignedAttribute("TileHeight", &tileHeight);

			m_Texture = CreateRef<SubTexture2D>(texture, tileWidth, tileHeight);
			m_TileProbabilities.resize(m_Texture->GetNumberOfCells());
		}
		else
			m_Texture = CreateRef<SubTexture2D>();

		tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("Tile");

		while (pTile)
		{
			int tileId = atoi(pTile->Attribute("Id"));
			const char* probability = pTile->Attribute("Probability");
			if (probability != nullptr)
				m_TileProbabilities[tileId] = atof(probability);
			pTile = pTile->NextSiblingElement("Tile");
		}

		tinyxml2::XMLElement* pAnimation = pRoot->FirstChildElement("Animation");
		while (pAnimation)
		{
			const char* name = pAnimation->Attribute("Name");

			double holdTime = 0.0f;
			uint32_t startFrame, frameCount;

			pAnimation->QueryDoubleAttribute("HoldTime", &holdTime);
			pAnimation->QueryUnsignedAttribute("StartFrame", &startFrame);
			pAnimation->QueryUnsignedAttribute("FrameCount", &frameCount);

			AddAnimation(name, startFrame, frameCount, holdTime);

			pAnimation = pAnimation->NextSiblingElement("Animation");
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

	for (auto& [name, animation] : m_Animations)
	{
		tinyxml2::XMLElement* pAnimation = pRoot->InsertNewChildElement("Animation");

		pAnimation->SetAttribute("Name", name.c_str());
		pAnimation->SetAttribute("StartFrame", animation.GetStartFrame());
		pAnimation->SetAttribute("FrameCount", animation.GetFrameCount());
		pAnimation->SetAttribute("HoldTime", animation.GetHoldTime());
	}

	for (size_t i = 0; i < m_TileProbabilities.size(); i++)
	{
		if (m_TileProbabilities[i] > 0.0)
		{
			tinyxml2::XMLElement* pTile = pRoot->InsertNewChildElement("Tile");
			pTile->SetAttribute("Id", i);
			pTile->SetAttribute("Probability", m_TileProbabilities[i]);
		}
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

void Tileset::SetCurrentTile(uint32_t tile)
{
	if (m_Texture)
	{
		m_Texture->SetCurrentCell(tile - 1);
		if (m_Animations.find(m_CurrentAnimation) != m_Animations.end())
		{
			m_Animations[m_CurrentAnimation].Start();
		}
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

void Tileset::SetTileProbability(size_t tile, double probability)
{
	if (m_TileProbabilities.size() > tile)
		m_TileProbabilities[tile] = probability;
}

void Tileset::SetSubTexture(Ref<SubTexture2D> subTexture)
{
	m_Texture = subTexture;
	if (m_Texture)
	{
		if (m_TileProbabilities.size() != m_Texture->GetNumberOfCells())
			m_TileProbabilities.resize(m_Texture->GetNumberOfCells());
	}
}