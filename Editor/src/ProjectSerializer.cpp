#include "ProjectSerializer.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"
#include "Utilities/SerializationUtils.h"

namespace ProjectSerializer
{
	bool Serialize(const ProjectData& data, const std::filesystem::path& filepath)
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* pRoot = doc.NewElement("Project");
		doc.InsertFirstChild(pRoot);

		pRoot->SetAttribute("EngineVersion", VERSION);

		if (!data.defaultScene.empty())
			SerializationUtils::Encode(pRoot->InsertNewChildElement("DefaultScene"), std::filesystem::path(data.defaultScene));

		pRoot->InsertNewChildElement("Description")->SetText(data.description.c_str());
		pRoot->InsertNewChildElement("SpriteAtlasPageSize")->SetText(data.spriteAtlasPageSize);

		return doc.SaveFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS;
	}

	bool Deserialize(ProjectData& data, const std::filesystem::path& filepath)
	{
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filepath.string().c_str()) != tinyxml2::XML_SUCCESS)
		{
			ENGINE_ERROR("Could not load project file: {0}, {1} on line {2}", filepath.string(), doc.ErrorName(), doc.ErrorLineNum());
			return false;
		}

		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Project");
		if (!pRoot)
		{
			ENGINE_ERROR("Not a valid project file. Could not find Project node");
			return false;
		}

		if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
			ENGINE_WARN("Loading a project file created with a different version of the engine");

		data = ProjectData();

		std::filesystem::path defaultScene;
		SerializationUtils::Decode(pRoot->FirstChildElement("DefaultScene"), defaultScene);
		data.defaultScene = defaultScene.string();

		if (const tinyxml2::XMLElement* pDescription = pRoot->FirstChildElement("Description"); pDescription && pDescription->GetText())
			data.description = pDescription->GetText();

		data.spriteAtlasPageSize = pRoot->FirstChildElement("SpriteAtlasPageSize") ? (uint32_t)pRoot->FirstChildElement("SpriteAtlasPageSize")->IntText(2048) : 2048;

		return true;
	}
}
