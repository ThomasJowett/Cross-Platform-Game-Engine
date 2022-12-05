#include "stdafx.h"
#include "SpriteSheet.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"
#include "Utilities/SerializationUtils.h"
#include "Logging/Instrumentor.h"

SpriteSheet::SpriteSheet()
{

}

SpriteSheet::SpriteSheet(const std::filesystem::path& filepath)
{
	Load(filepath);
}

bool SpriteSheet::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	if (!std::filesystem::exists(filepath)) return false;

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("SpriteSheet");

		if (!pRoot)
		{
			ENGINE_ERROR("SpriteSheet does not contain a SpriteSheet node: {0}", filepath);
			return false;
		}

		if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION) {
			ENGINE_WARN("Tileset created with a different version of the engine");
		}

		m_Filepath = filepath;

		m_Animations.clear();

		Ref<Texture2D> texture;

		SerializationUtils::Decode(pRoot->FirstChildElement("Texture"), texture);

		if (texture)
		{
			uint32_t spriteWidth, spriteHeight;
			pRoot->QueryUnsignedAttribute("SpriteWidth", &spriteWidth);
			pRoot->QueryUnsignedAttribute("SpriteHeight", &spriteHeight);

			m_Texture = CreateRef<SubTexture2D>(texture, spriteWidth, spriteHeight);
		}
		else
			m_Texture = CreateRef<SubTexture2D>();

		tinyxml2::XMLElement* pAnimation = pRoot->FirstChildElement("Animation");
		while (pAnimation)
		{
			const char* name = pAnimation->Attribute("Name");

			float holdTime = 0.0f;
			uint32_t startFrame, frameCount;

			pAnimation->QueryFloatAttribute("HoldTime", &holdTime);
			pAnimation->QueryUnsignedAttribute("StartFrame", &startFrame);
			pAnimation->QueryUnsignedAttribute("FrameCount", &frameCount);

			AddAnimation(name, startFrame, frameCount, holdTime);

			pAnimation = pAnimation->NextSiblingElement("Animation");
		}
	}
	else
	{
		ENGINE_ERROR("Could not load spritesheet {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	return true;
}

bool SpriteSheet::Save() const
{
	return SaveAs(m_Filepath);
}

bool SpriteSheet::SaveAs(const std::filesystem::path& filepath) const
{
	PROFILE_FUNCTION();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("SpriteSheet");

	pRoot->SetAttribute("EngineVersion", VERSION);

	if (m_Texture)
	{
		pRoot->SetAttribute("SpriteWidth", m_Texture->GetSpriteWidth());
		pRoot->SetAttribute("SpriteHeight", m_Texture->GetSpriteHeight());
	}

	doc.InsertFirstChild(pRoot);

	if (m_Texture && m_Texture->GetTexture())
	{
		SerializationUtils::Encode(pRoot->InsertNewChildElement("Texture"), m_Texture->GetTexture());
	}

	for (auto&& [name, animation] : m_Animations)
	{
		tinyxml2::XMLElement* pAnimation = pRoot->InsertNewChildElement("Animation");

		pAnimation->SetAttribute("Name", name.c_str());
		pAnimation->SetAttribute("StartFrame", animation.GetStartFrame());
		pAnimation->SetAttribute("FrameCount", animation.GetFrameCount());
		pAnimation->SetAttribute("HoldTime", animation.GetHoldTime());
	}

	tinyxml2::XMLError error = doc.SaveFile(filepath.string().c_str());
	return error == tinyxml2::XML_SUCCESS;
}

void SpriteSheet::AddAnimation(std::string name, uint32_t startFrame, uint32_t frameCount, float holdTime)
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
	m_Animations.insert({ name, Animation(startFrame, frameCount, holdTime) });
}

void SpriteSheet::RemoveAnimation(std::string name)
{
	m_Animations.erase(name);
	if (m_CurrentAnimation == name)
	{
		m_CurrentAnimation = m_Animations.begin()->first;
	}
}

void SpriteSheet::RenameAnimation(const std::string& oldName, const std::string& newName)
{
	if (m_Animations.find(newName) == m_Animations.end())
	{
		auto node = m_Animations.extract(oldName);
		if (!node.empty())
		{
			node.key() = newName;
			m_Animations.insert(std::move(node));
		}
		if (m_CurrentAnimation == oldName)
			m_CurrentAnimation = newName;
	}
}

void SpriteSheet::Animate(float deltaTime)
{
	if (m_Animations.find(m_CurrentAnimation) != m_Animations.end())
	{
		m_Animations[m_CurrentAnimation].Update(deltaTime, m_Texture);
	}
}

void SpriteSheet::SelectAnimation(const std::string& animationName)
{
	if (m_Animations.find(animationName) != m_Animations.end())
	{
		if (m_Animations.at(animationName).GetStartFrame() + m_Animations.at(animationName).GetFrameCount()
			<= m_Texture->GetNumberOfCells())
		{
			m_Texture->SetCurrentCell(m_Animations.at(animationName).GetStartFrame());
			m_CurrentAnimation = animationName;
		}
	}
}

void SpriteSheet::SetSubTexture(Ref<SubTexture2D> subTexture)
{
	m_Texture = subTexture;
	if (m_Animations.find(m_CurrentAnimation) != m_Animations.end())
		if (m_Animations[m_CurrentAnimation].GetCurrentFame() > subTexture->GetNumberOfCells())
			m_CurrentAnimation.clear();
}
