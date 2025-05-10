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
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath)) return false;

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(absolutePath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		if (!LoadXML(&doc))
		{
			ENGINE_ERROR("Could not load spritesheet file: {0}", absolutePath);
			return false;
		}
	}
	else
	{
		ENGINE_ERROR("Could not load spritesheet {0}. {1} on line {2}", absolutePath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}
	m_Filepath = filepath;
	return true;
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
	}
}

void SpriteSheet::SetSubTexture(Ref<SubTexture2D> subTexture)
{
	m_Texture = subTexture;
}

Animation* SpriteSheet::GetAnimation(const std::string& animationName)
{
	if (auto animation = m_Animations.find(animationName); animation != m_Animations.end())
	{
		return &animation->second;
	}
	return nullptr;
}
